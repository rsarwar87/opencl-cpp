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
    m_err = 0;
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
    DeviceProgram* ptr = (DeviceProgram *)FindProgram(nprog);
    ptr->CreateKernel(nkernel);

    for (size_t i = 0; i < args.size(); i++) {
      if (args.at(i) == "NULL") {
        ptr->AssignLocalArgument(nkernel, i, sz.at(i));
        continue;
      }
      DeviceBuffer* p_buf = (DeviceBuffer*)FindBuffer(args.at(i));
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
    ((DeviceProgram*) FindProgram(nprog))->RunKernel(nkernel, dim_sz, dim, ev, m_queues.at(qidx),
                                  func, n_ev, w_ev, msg);
    if (blocking) KernelWaitTillFinish(qidx);
  }

  void KernelWaitTillFinish(size_t qidx) {
    clFinish(m_queues.at(qidx));
  }

  cl_mem& CreateBuffer(std::string name, size_t sz, size_t qidx = 0, void* data = NULL,
                       MemType mem_type = READWRITE, bool pinned = true, bool sync = true,
                       bool blocking = true) {
    CheckIfInitialized();
    if (pinned && data != NULL) m_err = -999;
    CHECKERROR("Pinned memory must have NULL data pointer to return the allocated ptr");

    DeviceBuffer* ptr = new DeviceBuffer(m_ctx, m_queues.at(qidx), mem_type, 
                                        m_profiling, m_hostnotification);
    if (pinned) ptr->AppendFlag(ALCHOSTPTR);

    ptr->SetHostBuffer(pinned ? NULL : data, sz);
    ptr->CreateDeviceBuffer(sz);
    if (data != NULL && pinned == false) {
      if (sync)
      {
       if (mem_type != WRITEONLY) ptr->SyncDeviceBuffer(blocking);
       else ptr->SyncHostBuffer(blocking);
      }
    }
    m_buffers.push_back(std::make_pair(name, ptr));
    return ptr->GetDevBuffer();
  }

  void ReleaseBuffer(std::string name)
  {
    ((DeviceBuffer*) FindBuffer(name))->ReleaseMemory();
  }

  void * CreateMappedBuffer(std::string name,                           
                             bool blocking = true, cl_uint wcount = 0,
                             cl_event* ev = NULL, callbacktype* func = NULL,
                             size_t offset = 0) {
    return ((DeviceBuffer*) FindBuffer(name))->CreateMappedBuffer(blocking, wcount, ev, offset, func);
  }

  void SyncBuffer(std::string name, bool c2h = true, bool blocking = true,
                  size_t offset = 0, cl_uint wcount = 0, cl_event* ev = NULL) {
    if (!c2h)
      ((DeviceBuffer*) FindBuffer(name))->SyncDeviceBuffer(blocking, offset, wcount, ev);
    else
      ((DeviceBuffer*) FindBuffer(name))->SyncHostBuffer(blocking, offset, wcount, ev);
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

  void* FindBuffer(std::string name) {
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

  void* FindProgram(std::string name) {
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
  cl_context m_ctx;                         // device context
  std::vector<cl_command_queue> m_queues;   // vector containing all the dues issued against the device
  cl_device_id* m_device;                   // device pointer to the device being used
  cl_context_properties m_ctx_prop[3];      // properties related to the device context
  cl_int m_err;                             // error buffer
  bool m_initialized;                       // flag indicating that the device has been initialized
  std::vector<std::pair<std::string, void*>> m_programs; // list of all programs
  std::vector<std::pair<std::string, void*>> m_buffers;  // llist of all memory buffers
  bool m_profiling;                         // flag indicating that the device profiling is active
  bool m_hostnotification;                  // flag indicating that the host callback is active

  void SelectDevice(cl_device_type typ, size_t p_idx = 99, size_t d_idx = 99) {
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
      m_err = -999;
      CHECKERROR(ss.str());
    } else if (p_idx == 99 || d_idx == 99) {
      m_err = -999;
      ss << "invalid parameters: p_idx= " << p_idx << " d_idx= " << d_idx
         << std::endl;
      CHECKERROR(ss.str());
    } else {
      if (m_platforms.m_nPlatforms < p_idx) {
        m_err = -999;
        ss << "invalid platform id: requested= " << p_idx
           << " available= " << m_platforms.m_nPlatforms << std::endl;
        CHECKERROR(ss.str());
      } else if (m_platforms.m_dev_count[p_idx] < d_idx) {
        m_err = -999;
        ss << "invalid device id: requested= " << p_idx
           << " available= " << m_platforms.m_dev_count[p_idx] << std::endl;
        CHECKERROR(ss.str());
      } else if (m_platforms.m_ddata[p_idx][d_idx].m_type.value != typ) {
        m_err = -999;
        ss << "invalid device type: requested= " << typ
           << " available= " << m_platforms.m_ddata[p_idx][d_idx].m_type.value
           << std::endl;
        CHECKERROR(ss.str());
      }
      m_device = &m_platforms.m_devs[p_idx][d_idx];
    }
  }

  void CheckIfInitialized() {
    if (!m_initialized) {
      m_err = -999;
      std::stringstream ss;
      ss << "Device not initialized" << std::endl;
      CHECKERROR(ss.str());
    }
  }
};
#endif
