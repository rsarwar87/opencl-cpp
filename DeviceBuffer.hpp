#ifndef DEVBUFFHPP
#define DEVBUFFHPP

#include <CL/cl.h>
#include <assert.h>
#include <DeviceError.hpp>
#include <iostream>


enum MemType {
  READONLY,
  WRITEONLY,
  READWRITE,
  CPYHOSTPTR,
  ALCHOSTPTR,
  USEHOSTPTR
};

class DeviceBuffer {
 public:
  DeviceBuffer(cl_context& ctx, cl_command_queue& queue,
               MemType flg = READWRITE)
      : m_context(ctx), m_cmdQueue(queue) {
    m_flags = 0;
    m_sizeHostBuffer = 0;
    m_sizeDevBuffer = 0;
    m_initialized = false;
    m_err = CL_SUCCESS;
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

  cl_int SyncDeviceBuffer(bool blocking, size_t offset = 0) {
    if (m_initialized == false) return false;
    assert(m_sizeDevBuffer - offset > 0);
    assert(m_sizeHostBuffer - offset > 0);
    assert(m_sizeHostBuffer - m_sizeDevBuffer <= 0);
    m_err = clEnqueueReadBuffer(
        m_cmdQueue, m_devBuffer, blocking ? CL_TRUE : CL_FALSE, offset,
        m_sizeDevBuffer - offset, m_ptrHostBuffer, 0, NULL, NULL);
    CHECKERROR("Call made to clEnqueueReadBuffer.");
    return m_err;
  };
  cl_int SyncHostBuffer(cl_bool blocking, size_t offset = 0) {
    if (m_initialized == false) return false;
    assert(m_sizeDevBuffer - offset > 0);
    assert(m_sizeHostBuffer - offset > 0);
    assert(m_sizeDevBuffer - m_sizeHostBuffer <= 0);
    m_err = clEnqueueWriteBuffer(
        m_cmdQueue, m_devBuffer, blocking ? CL_TRUE : CL_FALSE, offset,
        m_sizeHostBuffer - offset, m_ptrHostBuffer, 0, NULL, NULL);
    CHECKERROR("Call made to clEnqueueWriteBuffer.");
    return m_err;
  };
  cl_int CopyBufferTo(cl_mem& dst, size_t sz, size_t offset_src = 0,
                      size_t offset_dst = 0) {
    if (m_initialized == false) return false;
    assert(sz + offset_src <= m_sizeDevBuffer);
    m_err = clEnqueueCopyBuffer(m_cmdQueue, m_devBuffer, dst, offset_src,
                                offset_dst, sz, 0, NULL, NULL);
    CHECKERROR("Call made to clEnqueueCopyBuffer.");
    return m_err;
  };

  cl_mem& GetDevBuffer() { return m_devBuffer; }
  void* GetHostBufferPtr() { return m_ptrHostBuffer; }
  void SetHostBuffer(void* mem, size_t sz) {
    m_sizeHostBuffer = sz;
    m_ptrHostBuffer = mem;
  }

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
};

#endif
