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
#include <utility>
#include <vector>

using namespace oclParser;

class DeviceHandler : public DeviceClass {
 public:
  DeviceHandler(bool profiling = false, bool hostnotification = false)
      : DeviceClass() {
    m_profiling = profiling;
    m_hostnotification = hostnotification;
  }

  ~DeviceHandler() {
    if (m_initialized) {
      for (size_t i = 0; i < m_queues.size(); i++)
        clReleaseCommandQueue(m_queues.at(i));         // Release context.
    }
    for (size_t i = 0; i < m_programs.size(); i++)
      if (m_programs.at(i).second != NULL) free(m_programs.at(i).second);
    for (size_t i = 0; i < m_buffers.size(); i++)
      if (m_buffers.at(i).second != NULL) free(m_buffers.at(i).second);
  }

  void CreateProgram(std::string name, std::string fname,
                     std::string compiler_flag = "-Werror",
                     bool print = false) {
    CheckIfInitialized();
    DeviceProgram* ptr = new DeviceProgram(m_ctx, m_device, fname,
                                           m_profiling, m_hostnotification);
    m_programs.push_back(std::make_pair(name, ptr));

    ptr->CreateProgramWithSource(print);
    ptr->BuildProgram(compiler_flag);
  }

  void CreateKernel(std::string nprog, std::string nkernel,
                    std::vector<std::string> args,
                    std::vector<size_t> sz = {}) {
    DeviceProgram* ptr = FindProgram(nprog);
    ptr->CreateKernel(nkernel);

    for (size_t i = 0; i < args.size(); i++) {
      if (args.at(i) == "NULL") {
        ptr->AssignLocalArgument(nkernel, i, sz.at(i));
        continue;
      }
      DeviceBuffer* p_buf = FindBuffer(args.at(i));
      ptr->AssignArgument<decltype(p_buf->GetDevBuffer())>(
          nkernel, i, p_buf->GetDevBuffer());
    }
  }

  void CreateUserEvent(cl_event &ev)
  {
    ev = clCreateUserEvent(m_ctx, &m_err);
    CHECKERROR("Creating User event ");
  }

  void RunKernel(std::string nprog, std::string nkernel, cl_uint dim_sz,
                 std::array<size_t*, 3> dim, cl_event& ev, size_t qidx = 0,
                 callbacktype* func = NULL, bool blocking = false,
                 cl_uint n_ev = 0, cl_event* w_ev = NULL,
                 const char* msg = NULL) {
    FindProgram(nprog)->RunKernel(nkernel, dim_sz, dim, ev, m_queues.at(qidx),
                                  func, n_ev, w_ev, msg);
    if (blocking) KernelWaitTillFinish(qidx);
  }

  void KernelWaitTillFinish(size_t qidx) {
    clFinish(m_queues.at(qidx));
  }

  cl_mem& CreateBuffer(std::string name, size_t sz, size_t qidx = 0, void* data = NULL,
                       MemType mem_type = READWRITE, bool sync = true,
                       bool blocking = true) {
    CheckIfInitialized();
    DeviceBuffer* ptr = new DeviceBuffer(m_ctx, m_queues.at(qidx), mem_type);

    ptr->CreateDeviceBuffer(sz);
    if (data != NULL) {
      ptr->SetHostBuffer(data, sz);
      if (sync) ptr->SyncDeviceBuffer(blocking);
    }
    m_buffers.push_back(std::make_pair(name, ptr));
    return ptr->GetDevBuffer();
  }

  void SyncBuffer(std::string name, bool c2h = true, bool blocking = true,
                  size_t offset = 0) {
    if (!c2h)
      FindBuffer(name)->SyncDeviceBuffer(blocking, offset);
    else
      FindBuffer(name)->SyncHostBuffer(blocking, offset);
  }

  size_t PrepareContextCommandQueue(cl_device_type typ, bool in_order = false,
                                  size_t p_idx = 99, size_t d_idx = 99) {
    if (!m_initialized) {
      SelectDevice(typ, p_idx, d_idx);
      m_ctx = clCreateContext(0, 1, m_device, NULL, NULL, &m_err);
      CHECKERROR("Creating context");
    }
    cl_command_queue_properties prop = 0;
    prop = in_order ? CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE : prop;
    prop |= m_profiling ? CL_QUEUE_PROFILING_ENABLE : prop;
    m_queues.push_back(clCreateCommandQueue(m_ctx, m_device[0], prop, &m_err));
    CHECKERROR("Creating CommandQueue");
    m_initialized = true;
    return m_queues.size() - 1;
  }

  DeviceBuffer* FindBuffer(std::string name) {
    CheckIfInitialized();
    for (size_t i = 0; i < m_buffers.size(); i++) {
      if (m_buffers.at(i).first == name) {
        return m_buffers.at(i).second;
      }
    }
    m_err = -9999;
    name = "Unable to find buffer: " + name;
    CHECKERROR(name);
    return NULL;
  }

  DeviceProgram* FindProgram(std::string name) {
    CheckIfInitialized();
    for (size_t i = 0; i < m_programs.size(); i++) {
      if (m_programs.at(i).first == name) {
        return m_programs.at(i).second;
      }
    }
    m_err = -9999;
    name = "Unable to find program: " + name;
    CHECKERROR(name);
    return NULL;
  }

  void set_profiling(bool val) { m_profiling = val; }
  void set_hostnotification(bool val) { m_hostnotification = val; }

 private:
  cl_context m_ctx;
  std::vector<cl_command_queue> m_queues;
  cl_device_id* m_device;
  cl_context_properties m_ctx_prop[3];
  cl_int m_err;
  bool m_initialized;
  std::vector<std::pair<std::string, DeviceProgram*>> m_programs;
  std::vector<std::pair<std::string, DeviceBuffer*>> m_buffers;

  bool m_profiling;
  bool m_hostnotification;

  void SelectDevice(cl_device_type typ, size_t p_idx = 99, size_t d_idx = 99) {
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
      ss << "Could not find the requested device tupe: " << typ << std::endl;
      CHECKERROR(ss.str());
    } else if (p_idx == 99 || d_idx == 99) {
      ss << "invalid parameters: p_idx= " << p_idx << " d_idx= " << d_idx
         << std::endl;
      CHECKERROR(ss.str());
    } else {
      if (m_platforms.m_nPlatforms < p_idx) {
        ss << "invalid platform id: requested= " << p_idx
           << " available= " << m_platforms.m_nPlatforms << std::endl;
        CHECKERROR(ss.str());
      } else if (m_platforms.m_dev_count[p_idx] < d_idx) {
        ss << "invalid device id: requested= " << p_idx
           << " available= " << m_platforms.m_dev_count[p_idx] << std::endl;
        CHECKERROR(ss.str());
      } else if (m_platforms.m_ddata[p_idx][d_idx].m_type.value != typ) {
        ss << "invalid device type: requested= " << typ
           << " available= " << m_platforms.m_ddata[p_idx][d_idx].m_type.value
           << std::endl;
        CHECKERROR(ss.str());
      }
      m_device = &m_platforms.m_devs[p_idx][d_idx];
    }
  }

  void CheckIfInitialized() {
    m_err = -999;
    if (!m_initialized) {
      std::stringstream ss;
      ss << "Device not initialized" << std::endl;
      CHECKERROR(ss.str());
    }
  }
};
#endif
