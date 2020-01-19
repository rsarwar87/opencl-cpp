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
  mm->PrintPlatformData();
  mm->PrepareContextCommandQueue(CL_DEVICE_TYPE_GPU);
  mm->CreateProgram("Test1", fname);

  std::string input = "GdkknVnqkc";
	size_t strlength = input.size();
	size_t global_work_size[1] = { strlength };
	cout << "input string:" << endl;
	cout << input << endl;
	char *output = (char*)malloc(strlength + 1);
  
  mm->CreateBuffer("input", (strlength)* sizeof(char), (void*)input.c_str(), 
                   READWRITE);
  mm->CreateBuffer("output", (strlength + 1)* sizeof(char), (void*)output, 
                   WRITEONLY, false);
  mm->CreateKernel("Test1", "helloworld", {"input", "output"}); 
  mm->RunKernel("Test1", "helloworld", 1, {NULL, global_work_size, NULL});
  mm->SyncBuffer("output");
	output[strlength] = '\0'; //Add the terminal character to the end of output.
	cout << "\noutput string:" << endl;
	cout << output << endl;
  delete mm;
  std::cout << "Wait" << std::endl;
  return 0;
}

