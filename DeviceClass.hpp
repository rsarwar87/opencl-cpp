#ifndef DEVCLASSHPP
#define DEVCLASSHPP

#include <CL/cl.h>
#include <DeviceError.hpp>
#include <string.h>
#include <DeviceBuffer.hpp>
#include <DeviceProgram.hpp>
#include <OpenCLInfo.hpp>
#include <vector>

using namespace oclParser;

class DeviceClass {
 public:
  DeviceClass() {
    m_err = clGetPlatformIDs(0, NULL, &m_platforms.m_nPlatforms);
    CHECKERROR("Call made to clGetPlatformIDs.");

    m_platforms.m_ddata = new device_data*[m_platforms.m_nPlatforms];
    m_platforms.m_pdata = new platform_data[m_platforms.m_nPlatforms];
    m_platforms.m_platformList = new cl_platform_id[m_platforms.m_nPlatforms];
    m_platforms.m_dev_count = new cl_uint[m_platforms.m_nPlatforms];
    m_platforms.m_devs = new cl_device_id*[m_platforms.m_nPlatforms];

    m_err = clGetPlatformIDs(m_platforms.m_nPlatforms,
                             m_platforms.m_platformList, NULL);
    CHECKERROR("Call made to clGetPlatformIDs.");
    for (uint32_t p = 0; p < m_platforms.m_nPlatforms; ++p) {
      GetPlatformData(p);

      m_err = clGetDeviceIDs(m_platforms.m_platformList[p], CL_DEVICE_TYPE_ALL,
                             0, NULL, &m_platforms.m_dev_count[p]);
      CHECKERROR("Call made to clGetDeviceIDs.");
      
      m_platforms.m_devs[p] = new cl_device_id[m_platforms.m_dev_count[p]];
      m_platforms.m_ddata[p] = new device_data[m_platforms.m_dev_count[p]];

      m_err = clGetDeviceIDs(m_platforms.m_platformList[p], CL_DEVICE_TYPE_ALL,
                             m_platforms.m_dev_count[p], m_platforms.m_devs[p],
                             NULL);
      CHECKERROR("Call made to clGetDeviceIDs.");

      for (uint32_t d = 0; d < m_platforms.m_dev_count[p]; d++) {
        GetDeviceData(p, d);
      }
    }
  }

  void PrintPlatformData() {
    for (uint32_t p = 0; p < m_platforms.m_nPlatforms; ++p) {
      std::cout << "Platform Name[" << p << "]: " << m_platforms.m_pdata[p].pname
                << std::endl;
      std::cout << "Platform Vendor[" << p << "]: "
                << m_platforms.m_pdata[p].pvendor << std::endl;
      std::cout << "Platform Version[" << p << "]: "
                << m_platforms.m_pdata[p].pversion << std::endl;
      std::cout << "Platform Profile[" << p << "]: "
                << m_platforms.m_pdata[p].pprofile << std::endl;
      std::cout << "Platform Extension[" << p << "]: "
                << m_platforms.m_pdata[p].pextension << std::endl;
    }
  }
  ~DeviceClass() {
    clReleaseCommandQueue(m_cmdQueue);  // Release  Command queue.
    clReleaseContext(m_context);        // Release context.

    if (m_device != NULL) {
      free(m_device);
      m_device = NULL;
    }
  }

 private:
  cl_context m_context;
  cl_command_queue m_cmdQueue;
  cl_device_id* m_device;

  platform_list m_platforms;

  cl_int m_err;
  /*
  https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/enums.html#cl_device_info
  */

  void GetDeviceData(uint32_t p, uint32_t d) {}
  void GetPlatformData(uint32_t p) {
    m_platforms.m_pdata[p].pname =
        GetPlatformInfo(m_platforms.m_platformList[p], CL_PLATFORM_NAME);
    m_platforms.m_pdata[p].pversion =
        GetPlatformInfo(m_platforms.m_platformList[p], CL_PLATFORM_VERSION);
    m_platforms.m_pdata[p].pprofile =
        GetPlatformInfo(m_platforms.m_platformList[p], CL_PLATFORM_PROFILE);
    m_platforms.m_pdata[p].pvendor =
        GetPlatformInfo(m_platforms.m_platformList[p], CL_PLATFORM_VENDOR);
    m_platforms.m_pdata[p].pextension =
        GetPlatformInfo(m_platforms.m_platformList[p], CL_PLATFORM_EXTENSIONS);
  }

  std::string GetPlatformInfo(cl_platform_id platform, cl_platform_info param) {
    size_t buffer_size;
    m_err = clGetPlatformInfo(platform, param, 0, NULL, &buffer_size);
    CHECKERROR("Call made to clGetPlatformInfo.");

    char* buffer = (char*)malloc(buffer_size);
    m_err = clGetPlatformInfo(platform, param, buffer_size, buffer, NULL);
    CHECKERROR("Call made to clGetPlatformInfo.");

    std::string ret(buffer);
    free(buffer);
    return ret;
  };
  /*
   *CL_PLATFORM_PROFILE
  CL_PLATFORM_VERSION
  CL_PLATFORM_NAME
  CL_PLATFORM_VENDOR
  CL_PLATFORM_EXTENSIONS<Paste>
   * */
  std::string printDeviceInfo(cl_device_id device, cl_device_info param) {
    size_t buffer_size;
    m_err = clGetDeviceInfo(device, param, 0, NULL, &buffer_size);
    CHECKERROR("Call made to clGetDeviceInfo.");

    char* buffer = (char*)malloc(buffer_size);
    m_err = clGetDeviceInfo(device, param, buffer_size, buffer, NULL);
    CHECKERROR("Call made to clGetDeviceInfo.");

    std::string ret(buffer);
    free(buffer);
    return ret;
  };
};
#endif
