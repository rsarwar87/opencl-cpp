#ifndef DEVHANDLERHPP
#define DEVHANDLERHPP

#include <CL/cl.h>
#include <string.h>
#include <DeviceClass.hpp>
#include <DeviceBuffer.hpp>
#include <DeviceError.hpp>
#include <DeviceProgram.hpp>
#include <OpenCLInfo.hpp>
#include <vector>

using namespace oclParser;

class DeviceHandler : public DeviceClass {
 public:
  DeviceHandler() : DeviceClass() {
  }

  ~DeviceHandler() {
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
  cl_device_id *m_device;


  cl_int m_err;

};
#endif
