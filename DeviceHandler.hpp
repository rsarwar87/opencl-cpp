#ifndef DEVHANDLERHPP
#define DEVHANDLERHPP

#include <CL/cl.h>
#include <string.h>
#include <DeviceBuffer.hpp>
#include <DeviceClass.hpp>
#include <DeviceError.hpp>
#include <DeviceProgram.hpp>
#include <OpenCLInfo.hpp>
#include <tuple>
#include <vector>
#include <utility>

using namespace oclParser;

class DeviceHandler : public DeviceClass {
 public:
  DeviceHandler() : DeviceClass() {}

  ~DeviceHandler() {
    if (m_initizalized) {
      clReleaseCommandQueue(m_queue);  // Release  Command queue.
      clReleaseContext(m_ctx);         // Release context.
    }
  }

  void create_program(std::string name){

  }

  void create_buffer(std::string name, size_t sz, void* data){

  }

  void prepare_context_commandqueue(cl_device_type typ, bool in_order = false,
                                    bool profiling = false, size_t p_idx = 99,
                                    size_t d_idx = 99) {
    select_device(typ, p_idx, d_idx);
    m_ctx = clCreateContext(0, 1, m_device, NULL, NULL, &m_err);
    CHECKERROR("Creating context");

    cl_command_queue_properties prop = 0;
    prop = in_order ? CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE : prop;
    prop = profiling ? CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE : prop;
    m_queue = clCreateCommandQueue(m_ctx, m_device[0], prop, &m_err);
    CHECKERROR("Creating CommandQueue");
    m_initizalized = true;
  }

 private:
  cl_context m_ctx;
  cl_command_queue m_queue;
  cl_device_id *m_device;
  cl_context_properties m_ctx_prop[3];
  cl_int m_err;
  bool m_initizalized;
  std::vector<std::pair<std::string, DeviceProgram*>> m_programs;
  std::vector<std::pair<std::string, DeviceBuffer*>> m_buffers;

  void select_device(cl_device_type typ, size_t p_idx = 99, size_t d_idx = 99) {
    m_err = -999;
    std::stringstream ss;
    if (p_idx == 99 && d_idx == 99) {
      for (uint32_t p = 0; p < m_platforms.m_nPlatforms; ++p) {
        for (uint32_t d = 0; d < m_platforms.m_dev_count[p]; d++) {
          if (m_platforms.m_ddata[p][d].m_type.value == typ) {
            m_device = &m_platforms.m_devs[p][d];
            return;
          }
        }
      }
      ss << "Could not find the requested device tupe: " << typ;
      CHECKERROR(ss.str());
    } else if (p_idx == 99 || d_idx == 99) {
      ss << "invalid parameters: p_idx= " << p_idx << " d_idx= " << d_idx;
      CHECKERROR(ss.str());
    } else {
      if (m_platforms.m_nPlatforms < p_idx) {
        ss << "invalid platform id: requested= " << p_idx
           << " available= " << m_platforms.m_nPlatforms;
        CHECKERROR(ss.str());
      } else if (m_platforms.m_dev_count[p_idx] < d_idx) {
        ss << "invalid device id: requested= " << p_idx
           << " available= " << m_platforms.m_dev_count[p_idx];
        CHECKERROR(ss.str());
      } else if (m_platforms.m_ddata[p_idx][d_idx].m_type.value != typ) {
        ss << "invalid device type: requested= " << typ
           << " available= " << m_platforms.m_ddata[p_idx][d_idx].m_type.value;
        CHECKERROR(ss.str());
      }
      m_device = &m_platforms.m_devs[p_idx][d_idx];
    }
  }

  DeviceBuffer* find_buffer(std::string name){
    for (size_t i = 0; i < m_buffers.size(); i++)
    {
      if (m_buffers.at(i).first == name)
      {
        return m_buffers.at(i).second;
      }
    }
    m_err = -9999;
    name = "Unable to find buffer: " + name;
    CHECKERROR(name);
    return NULL;
  }

  DeviceProgram* find_program(std::string name){
    for (size_t i = 0; i < m_programs.size(); i++)
    {
      if (m_programs.at(i).first == name)
      {
        return m_programs.at(i).second;
      }
    }
    m_err = -9999;
    name = "Unable to find program: " + name;
    CHECKERROR(name);
    return NULL;
  }

};
#endif
