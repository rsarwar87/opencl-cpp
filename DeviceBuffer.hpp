#ifndef DEVBUFFHPP
#define DEVBUFFHPP

#include <CL/cl.h>
#include <assert.h>
#include <DeviceError.hpp>
#include <iostream>
#include <chrono>
#include <DeviceCommon.hpp>

class DeviceBuffer {
 public:
  DeviceBuffer(cl_context& ctx, cl_command_queue& queue,
               MemType flg = READWRITE, bool profiling = true,
               bool hostnotification = false)
      : m_context(ctx), m_cmdQueue(queue) {
    m_flags = 0;
    m_sizeHostBuffer = 0;
    m_sizeDevBuffer = 0;
    m_initialized = false;
    m_map = false;
    m_err = CL_SUCCESS;
    m_profiling = profiling;
    m_hostnotification = hostnotification;
    m_imagesize = {0,0,0};
    m_imagepitch = {0,0};
    m_flags_map = 0;
    m_is3d = false;
    m_imageformat = {0,0};
    AppendFlag(flg);
  }

  ~DeviceBuffer() { 
    ReleaseMemory();
  }

  void ReleaseMemory()
  {
    if (!m_map) clReleaseMemObject(m_devBuffer);
    else clEnqueueUnmapMemObject(m_cmdQueue, m_devBuffer, m_ptrHostBuffer, 0, NULL, NULL);
  }
  void* CreateMappedBuffer(bool blocking = false, cl_uint wcount = 0,
                                 cl_event* ev = NULL, size_t offset = 0,
                                 callbacktype* func = NULL) {
    if (m_initialized == false) m_err = -9999;
    CHECKERROR("Memory not initialized.");
    m_ptrHostBuffer = clEnqueueMapBuffer(m_cmdQueue, m_devBuffer, blocking, m_flags_map,
                                   offset, m_sizeDevBuffer, wcount, ev, &m_event, &m_err);
    CHECKERROR("Call made to clEnqueueMapBuffer.");
    if (m_err == CL_SUCCESS) m_initialized = true;
    SetEventConfiguration(func, (void*)__func__);
    CHECKERROR("Memory not initialized.");
    m_map = true;
    return m_ptrHostBuffer;
  }
  cl_int CreateDeviceBuffer(bool is3d, cl_image_format format)
  {
    if (m_initialized == true) m_err = -9999;
    CHECKERROR("Memory already initialized.");
    assert(m_sizeDevBuffer > 0);
    m_is3d = is3d;
    m_imageformat = format;
    
    ValidateImageBufferSize();
    if (m_is3d)
      m_devBuffer = clCreateImage3D(
          m_context, m_flags, &m_imageformat, m_imagesize[0], m_imagesize[1],
          m_imagesize[2], m_imagepitch[0], m_imagepitch[1], NULL, &m_err);
    else
      m_devBuffer = clCreateImage2D(
          m_context, m_flags, &m_imageformat, m_imagesize[0], m_imagesize[1],
          m_imagepitch[0], NULL, &m_err);

    CHECKERROR("Call made to clCreateBuffer.");
    if (m_err == CL_SUCCESS) m_initialized = true;
    return m_err;

  }
  cl_int CreateDeviceBuffer(size_t sz) {
    if (m_initialized == true) m_err = -9999;
    CHECKERROR("Memory already initialized.");
    assert(sz > 0);
    m_sizeDevBuffer = sz;
    m_devBuffer =
        clCreateBuffer(m_context, m_flags, m_sizeDevBuffer, NULL, &m_err);
    CHECKERROR("Call made to clCreateBuffer.");
    if (m_err == CL_SUCCESS) m_initialized = true;
    return m_err;
  }

  void AppendFlag(MemType flg) {
    if (flg == READONLY)
    {
      m_flags |= CL_MEM_READ_ONLY;
      m_flags_map |= CL_MAP_READ;
    }
    else if (flg == WRITEONLY)
    {
      m_flags |= CL_MEM_WRITE_ONLY;
      m_flags_map |= CL_MAP_WRITE;
    }
    else if (flg == READWRITE)
    {
      m_flags |= CL_MEM_READ_WRITE;
      m_flags_map |= CL_MAP_WRITE;
      m_flags_map |= CL_MAP_READ;
    }
    else if (flg == CPYHOSTPTR)
      m_flags |= CL_MEM_COPY_HOST_PTR;
    else if (flg == ALCHOSTPTR)
      m_flags |= CL_MEM_ALLOC_HOST_PTR;
    else if (flg == USEHOSTPTR)
      m_flags |= CL_MEM_USE_HOST_PTR;
  }

  cl_int SyncDeviceBuffer(bool blocking, size_t offset = 0, cl_uint wcount = 0,
                          cl_event* ev = NULL, callbacktype* func = NULL) {
    if (m_initialized == false) m_err = -9999;
    CHECKERROR("Memory not initialized.");
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
    if (m_initialized == false) m_err = -9999;
    CHECKERROR("Memory not initialized.");
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
    if (m_initialized == false) m_err = -9999;
    CHECKERROR("Memory not initialized.");
    assert(sz + offset_src <= m_sizeDevBuffer);
    m_err = clEnqueueCopyBuffer(m_cmdQueue, m_devBuffer, dst, offset_src,
                                offset_dst, sz, wcount, ev, &m_event);
    CHECKERROR("Call made to clEnqueueCopyBuffer.");
    SetEventConfiguration(func, (void*)__func__);
    return m_err;
  };

  cl_mem& GetDevBuffer() { return m_devBuffer; }
  void* GetHostBufferPtr() { return m_ptrHostBuffer; }
  void SetHostBuffer(void* mem, size_t sz, std::array<size_t,3> dimensions = {0,0,0}, std::array<size_t, 2> pitch = {0, 0}) {
    m_imagesize = dimensions;
    m_imagepitch = pitch;
    m_sizeHostBuffer = sz;
    m_ptrHostBuffer = mem;
  }
  bool IsMap() { return m_map; }
  cl_event& GetEventClass() { return m_event; }

 private:
  cl_context& m_context;
  cl_command_queue& m_cmdQueue;
  cl_mem_flags m_flags;
  cl_map_flags m_flags_map;
  size_t m_sizeHostBuffer;
  std::array<size_t, 3> m_imagesize;
  std::array<size_t, 2> m_imagepitch;
  size_t m_sizeDevBuffer;
  cl_mem m_devBuffer;
  void* m_ptrHostBuffer;
  cl_int m_err;
  bool m_initialized;
  bool m_is3d;
  bool m_map;
  cl_image_format m_imageformat;
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
    //clReleaseEvent(e);
    //  if (e != NULL) delete e;
  }
  static void CL_CALLBACK profiling_callback(cl_event e, cl_int status,
                                             void* data) {
    using namespace std::chrono_literals;
    auto starttime = std::chrono::high_resolution_clock::now();
    cl_int m_err = CL_SUCCESS;
    cl_ulong start = 0, end = 0;
    m_err = clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_START,
                                    sizeof(cl_ulong), &start, NULL);
    CHECKERROR("READING START TIME");
    m_err = clGetEventProfilingInfo(e, CL_PROFILING_COMMAND_END,
                                    sizeof(cl_ulong), &end, NULL);
    CHECKERROR("READING ENDTIME TIME");
    printf("Average transfer, %s, time: %lu (%lu)\n", (const char*)data, end - start, starttime);

    //  if (e != NULL) delete e;
  }
  void ValidateImageBufferSize()
  {
    size_t ch = 0;
    size_t sz = 0;
    assert(m_imagesize[0] > 0 || m_imagesize[1] > 0);
    assert(m_imagepitch[0] > 0);
    if (m_is3d)
    {
      assert(m_imagepitch[1] > 0);
      assert(m_imagesize[2] > 0);
    }

    if (m_imageformat.image_channel_data_type ==
            CL_SNORM_INT8 || m_imageformat.image_channel_data_type ==
            CL_UNORM_INT8 || m_imageformat.image_channel_data_type ==
            CL_SIGNED_INT8 || m_imageformat.image_channel_data_type ==
            CL_UNSIGNED_INT8)
      sz = 1;
    else if (m_imageformat.image_channel_data_type ==
                 CL_SNORM_INT16 || m_imageformat.image_channel_data_type ==
                 CL_SNORM_INT16 || m_imageformat.image_channel_data_type ==
                 CL_SIGNED_INT16 || m_imageformat.image_channel_data_type ==
                 CL_UNSIGNED_INT16 || m_imageformat.image_channel_data_type ==
                 CL_HALF_FLOAT || m_imageformat.image_channel_data_type ==
                 CL_UNORM_SHORT_565 || m_imageformat.image_channel_data_type ==
                 CL_UNORM_SHORT_555 || m_imageformat.image_channel_data_type ==
                 CL_UNORM_INT_101010)
      sz = 2;
    else if (m_imageformat.image_channel_data_type ==
                 CL_SIGNED_INT32 || m_imageformat.image_channel_data_type ==
                 CL_UNSIGNED_INT32 || 
             m_imageformat.image_channel_data_type ==
                 CL_HALF_FLOAT)
      sz = 4;

    if (m_imageformat.image_channel_order == CL_R or m_imageformat.image_channel_order == CL_A)
      ch = 1;
    else if (m_imageformat.image_channel_order == CL_INTENSITY || m_imageformat.image_channel_order == CL_LUMINANCE ||
        m_imageformat.image_channel_order == CL_RGBA || m_imageformat.image_channel_order == CL_ARGB || m_imageformat.image_channel_order == CL_BGRA)
      ch = 4;
    else
      ch = 3;

    assert(ch*sz*m_imagesize[0]*m_imagesize[1]*m_imagesize[2] <= m_sizeHostBuffer);
  }
};

#endif
