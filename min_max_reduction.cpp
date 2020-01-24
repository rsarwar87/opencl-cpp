#include <DeviceHandler.hpp>
#include <exception>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
int main (int ac, char** av)
{
  std::string fname;
  try {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
        "filename", po::value<std::string>(&fname)->required(), "set kernel filename");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
      cout << desc << "\n";
      return 0;
    }
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return false;
  } catch (...) {
    std::cerr << "Unknown error!"
              << "\n";
    return false;
  }

  DeviceHandler* mm = new DeviceHandler();
  mm->set_profiling(true); 
//  mm->set_hostnotification(true); 
  mm->PrintPlatformData();
  size_t qidx = mm->PrepareContextCommandQueue(CL_DEVICE_TYPE_GPU);
  mm->CreateProgram("Test1", fname);

  const size_t nRow = 16;
  const size_t nCol = 16;
  const size_t nTotal = nCol * nRow;
  const size_t workGroupSize = nCol;
  const size_t nGroups = nTotal / workGroupSize;

  std::array<std::array<float, nCol>, nRow> arr;
  float tot[nRow] = {0};
  for (size_t i = 0; i < nRow; i++)
  for (size_t j = 0; j < nCol; j++)
  { 
    arr.at(i).at(j) = i + j+ 1;
    tot[i] += i + j + 1;
  }

  mm->CreateBuffer("input", nTotal* sizeof(float), qidx, NULL,// (void*)input.c_str(), 
                   READONLY, true);
  void *data = mm->CreateMappedBuffer("input", false);
  memcpy ( data, arr.data(), nTotal* sizeof(float));

  mm->CreateBuffer("pMin", nGroups* sizeof(float), qidx, NULL, 
                   WRITEONLY, true);
  void *pMin = mm->CreateMappedBuffer("pMin", false);
  mm->CreateBuffer("pMax", nGroups* sizeof(float), qidx, NULL, 
                   WRITEONLY, true);
  void *pMax = mm->CreateMappedBuffer("pMax", false);
  mm->CreateBuffer("pSum", nGroups* sizeof(float), qidx, NULL, 
                   WRITEONLY, true);
  void *pSum = mm->CreateMappedBuffer("pSum", false);

  size_t siz = workGroupSize*sizeof(float);
  mm->CreateKernel("Test1", "min_max_reduction_2d", {"input", "pMin", "pMax", "pSum", "NULL", "NULL", "NULL"}, {siz,siz,siz}); 
  const size_t _dim = 2;
  size_t sz[_dim] = {workGroupSize, nGroups};
  size_t lsz[_dim] = {workGroupSize, 1};
  cl_event ev[3];
  mm->CreateUserEvent(ev[0]);
  mm->RunKernel("Test1", "min_max_reduction_2d", _dim, {NULL, sz, sz}, ev[1], qidx, NULL, false, 1, &ev[0]);
  clSetUserEventStatus(ev[0], CL_SUCCESS);
  mm->SyncBuffer("pMin");
  mm->SyncBuffer("pMax");
  mm->SyncBuffer("pSum");
	cout << "\noutput :" << endl;
  for (size_t i = 0; i < nGroups; i++)
	  cout << i << " == " << ((float*)pMax)[i] << " " << ((float*)pMin)[i] << " " << ((float*)pSum)[i]  << " " << tot [i] << endl;
  delete mm;
  return 0;
}

