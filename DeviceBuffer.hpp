#ifndef DEVBUFFHPP
#define DEVBUFFHPP

#include <CL/cl.h>
#include <assert.h>
#include <DeviceError.hpp>
#include <iostream>

enum ObjType { Object, Image, Reat };

enum MemType {
  READONLY,
  WRITEONLY,
  READWRITE,
  CPYHOSTPTR,
  ALCHOSTPTR,
  USEHOSTPTR
};
typedef void (*callbacktype)(cl_event, cl_int, void*);

class DeviceBuffer {
 public:
  DeviceBuffer(cl_context& ctx, cl_command_queue& queue,
               MemType flg = READWRITE, bool map = false, bool profiling = true,
               bool hostnotification = false)
      : m_context(ctx), m_cmdQueue(queue) {
    m_flags = 0;
    m_sizeHostBuffer = 0;
    m_sizeDevBuffer = 0;
    m_initialized = false;
    m_map = map;
    m_err = CL_SUCCESS;
    m_profiling = profiling;
    m_hostnotification = hostnotification;
    AppendFlag(flg);
  }

  ~DeviceBuffer() { clReleaseMemObject(m_devBuffer); }

  cl_int CreateDeviceBuffer(size_t sz) {
    assert(sz > 0);
    assert(m_flags > 0);
    m_sizeDevBuffer = sz;
    m_devBuffer =
        clCreateBuffer(m_context, m_flags, m_sizeDevBuffer, NULL, &m_err);
    CHECKERROR("Call made to clCreateBuffer.");
    if (m_err == CL_SUCCESS) m_initialized = true;
    return m_err;
  }

  void AppendFlag(MemType flg) {
    if (flg == READONLY)
      m_flags |= CL_MEM_READ_ONLY;
    else if (flg == WRITEONLY)
      m_flags |= CL_MEM_WRITE_ONLY;
    else if (flg == READWRITE)
      m_flags |= CL_MEM_READ_WRITE;
    else if (flg == CPYHOSTPTR)
      m_flags |= CL_MEM_COPY_HOST_PTR;
    else if (flg == ALCHOSTPTR)
      m_flags |= CL_MEM_ALLOC_HOST_PTR;
    else if (flg == USEHOSTPTR)
      m_flags |= CL_MEM_COPY_HOST_PTR;
  }

  cl_int SyncDeviceBuffer(bool blocking, size_t offset = 0, cl_uint wcount = 0,
                          cl_event* ev = NULL, callbacktype* func = NULL) {
    if (m_initialized == false) return false;
    assert(m_sizeDevBuffer - offset > 0);
    assert(m_sizeHostBuffer - offset > 0);
    assert(m_sizeHostBuffer - m_sizeDevBuffer <= 0);
    assert(m_ptrHostBuffer != NULL);
    m_err = clEnqueueWriteBuffer(
        m_cmdQueue, m_devBuffer, blocking ? CL_TRUE : CL_FALSE, offset,
        m_sizeDevBuffer - offset, m_ptrHostBuffer, wcount, ev, &m_event);
    CHECKERROR("Call made to clEnqueueReadBuffer.");
    SetEventConfiguration(func, (void*)__func__);
    return m_err;
  };
  cl_int SyncHostBuffer(cl_bool blocking, size_t offset = 0, cl_uint wcount = 0,
                        cl_event* ev = NULL, callbacktype* func = NULL) {
    if (m_initialized == false) return false;
    assert(m_sizeDevBuffer - offset > 0);
    assert(m_sizeHostBuffer - offset > 0);
    assert(m_sizeDevBuffer - m_sizeHostBuffer <= 0);
    assert(m_ptrHostBuffer != NULL);
    m_err = clEnqueueReadBuffer(
        m_cmdQueue, m_devBuffer, blocking ? CL_TRUE : CL_FALSE, offset,
        m_sizeHostBuffer - offset, m_ptrHostBuffer, wcount, ev, &m_event);
    CHECKERROR("Call made to clEnqueueWriteBuffer.");
    SetEventConfiguration(func, (void*)__func__);
    return m_err;
  };
  cl_int CopyBufferTo(cl_mem& dst, size_t sz, size_t offset_src = 0,
                      size_t offset_dst = 0, cl_uint wcount = 0,
                      cl_event* ev = NULL, callbacktype* func = NULL) {
    if (m_initialized == false) return false;
    assert(sz + offset_src <= m_sizeDevBuffer);
    m_err = clEnqueueCopyBuffer(m_cmdQueue, m_devBuffer, dst, offset_src,
                                offset_dst, sz, wcount, ev, &m_event);
    CHECKERROR("Call made to clEnqueueCopyBuffer.");
    SetEventConfiguration(func, (void*)__func__);
    return m_err;
  };

  cl_mem& GetDevBuffer() { return m_devBuffer; }
  void* GetHostBufferPtr() { return m_ptrHostBuffer; }
  void SetHostBuffer(void* mem, size_t sz) {
    m_sizeHostBuffer = sz;
    m_ptrHostBuffer = mem;
  }
  bool IsMap() { return m_map; }
  cl_event& GetEventClass() { return m_event; }

 private:
  cl_context& m_context;
  cl_command_queue& m_cmdQueue;
  cl_mem_flags m_flags;
  size_t m_sizeHostBuffer;
  size_t m_sizeDevBuffer;
  cl_mem m_devBuffer;
  void* m_ptrHostBuffer;
  cl_int m_err;
  bool m_initialized;
  bool m_map;
  cl_event m_event;
  bool m_profiling;
  bool m_hostnotification;

  void SetEventConfiguration(callbacktype* func, void* data) {
    if (m_hostnotification == true) {
      if (func != NULL) {
        m_err = clSetEventCallback(m_event, CL_COMPLETE, *func, data);
        CHECKERROR("Call made to clSetEventCallback.");
      } else {
        m_err =
            clSetEventCallback(m_event, CL_COMPLETE, &default_callback, data);
        CHECKERROR("Call made to clSetEventCallback.");
      }
    } else if (m_profiling == true) {
      m_err =
          clSetEventCallback(m_event, CL_COMPLETE, &profiling_callback, data);
      CHECKERROR("Call made to clSetEventCallback.");
    }
  }

  static void CL_CALLBACK default_callback(cl_event e, cl_int status,
                                           void* data) {
    if (data == NULL)
      std::cout << "command completed" << status << std::endl;
    else
      std::cout << "command completed" << (char*)data << std::endl;
    clReleaseEvent(e);
    //  if (e != NULL) delete e;
  }
  static void CL_CALLBACK profiling_callback(cl_event e, cl_int status,
                                             void* data) {
    cl_int m_err = CL_SUCCESS;
    cl_ulong start = 0, end = 0;
    m_err = clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_START,
                                    sizeof(cl_ulong), &start, NULL);
    CHECKERROR("READING START TIME");
    m_err = clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_END,
                                    sizeof(cl_ulong), &end, NULL);
    CHECKERROR("READING ENDTIME TIME");
    printf("Average transfer, %s, time: %lu\n", (const char*)data, end - start);

    //  if (e != NULL) delete e;
  }
};

#endif
