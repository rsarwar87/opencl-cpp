#include <DeviceClass.hpp>
#include <iostream>
int main ()
{
  DeviceClass *mm = new DeviceClass();
  mm->PrintPlatformData();
  std::cout << "Wait" << std::endl;
    return 0;
}

