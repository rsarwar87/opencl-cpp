#include <DeviceHandler.hpp>
#include <iostream>
int main ()
{
  DeviceHandler *mm = new DeviceHandler();
  mm->PrintPlatformData();
  std::cout << "Wait" << std::endl;
    return 0;
}

