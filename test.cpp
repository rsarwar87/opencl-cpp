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
  size_t qidx = mm->PrepareContextCommandQueue(CL_DEVICE_TYPE_CPU);
  mm->CreateProgram("Test1", fname);

  std::string input = "GdkknVnqkc";
	cout << "input string:" << endl;
	cout << input << endl;
	char *output = (char*)malloc(input.size() + 1);
  mm->CreateBuffer("input", (input.size())* sizeof(char), qidx, NULL, 
                   READONLY, true);
  void *data = mm->CreateMappedBuffer("input", false);
  memcpy ( data, input.c_str(), (input.size())* sizeof(char));
  mm->CreateBuffer("inter", (input.size() + 1)* sizeof(char), qidx, NULL, 
                   READWRITE, false);
  mm->CreateBuffer("output", (input.size() + 1)* sizeof(char), qidx, (void*)output, 
                   WRITEONLY, false);
  mm->CreateKernel("Test1", "helloworld", {"input", "inter"}); 
  mm->CreateKernel("Test1", "helloworld2", {"inter", "output"}); 
  size_t sz[1] = {input.size()};
  cl_event ev[3];
  mm->CreateUserEvent(ev[0]);
  mm->RunKernel("Test1", "helloworld", 1, {NULL, sz, NULL}, ev[1], qidx, NULL, false, 1, &ev[0]);
  mm->RunKernel("Test1", "helloworld2", 1, {NULL, sz, NULL}, ev[2], qidx, NULL, false, 1, &ev[1]);
  clSetUserEventStatus(ev[0], CL_SUCCESS);
  mm->SyncBuffer("output");
	output[input.size()] = '\0'; //Add the terminal character to the end of output.
	cout << "\noutput string:" << endl;
	cout << output << endl;
  delete mm;
  return 0;
}

