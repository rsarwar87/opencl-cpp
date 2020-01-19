#include <DeviceHandler.hpp>
#include <iostream>
int main ()
{
  DeviceHandler *mm = new DeviceHandler();
  mm->PrintPlatformData();
  mm->prepare_context_commandqueue(CL_DEVICE_TYPE_GPU);
  delete mm;
  std::cout << "Wait" << std::endl;
    return 0;
}

