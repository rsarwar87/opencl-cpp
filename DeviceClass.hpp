#ifndef DEVCLASSHPP
#define DEVCLASSHPP

#include <CL/cl.h>
#include <string.h>
#include <DeviceError.hpp>
#include <OpenCLInfo.hpp>
#include <vector>

using namespace oclParser;

class DeviceClass {
 public:
  DeviceClass() {
    m_err = clGetPlatformIDs(0, NULL, &m_platforms.m_nPlatforms);
    CHECKERROR("Call made to clGetPlatformIDs.");

    m_platforms.m_ddata = new device_data *[m_platforms.m_nPlatforms];
    m_platforms.m_pdata = new platform_data[m_platforms.m_nPlatforms];
    m_platforms.m_platformList = new cl_platform_id[m_platforms.m_nPlatforms];
    m_platforms.m_dev_count = new cl_uint[m_platforms.m_nPlatforms];
    m_platforms.m_devs = new cl_device_id *[m_platforms.m_nPlatforms];

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
        GetDeviceData(m_platforms.m_platformList[p], &m_platforms.m_devs[p][d],
                      m_platforms.m_ddata[p][d]);
      }
    }
  }

  void PrintDeviceData(uint32_t id, struct device_data &data) {
#define PRINT(x)                                               \
  std::cout << "\t[" << id << "]: " << data.m_##x.name << ": " \
            << data.m_##x.value << std::endl;
    PRINT(name);
    PRINT(opencl_c_version)
    PRINT(extensions)
    PRINT(profile)
    PRINT(vendor_id)
    PRINT(vendor)
    PRINT(version)
    PRINT(max_compute_units);
    PRINT(max_clock_frequency);
    PRINT(max_read_image_args);
    PRINT(endian_little);
    PRINT(address_bits);
    PRINT(available)
    PRINT(compiler_available)
    PRINT(error_correction_support)

    PRINT(global_mem_cache_size)
    PRINT(global_mem_cache_type)
    PRINT(global_mem_cacheline_size)
    PRINT(global_mem_size)
    PRINT(host_unified_memory)
    PRINT(image_support)
    PRINT(image3d_max_depth)
    PRINT(image3d_max_height)
    PRINT(image3d_max_width)
    PRINT(image2d_max_height)
    PRINT(image2d_max_width)
    PRINT(local_mem_size)
    PRINT(max_parameter_size)
    PRINT(max_work_group_size)
    PRINT(profiling_timer_resolution)
    PRINT(max_constant_args)
    PRINT(max_samplers)
    PRINT(max_work_item_dimensions)
    PRINT(mem_base_addr_align)
    PRINT(min_data_type_align_size)
    PRINT(max_constant_buffer_size)
    PRINT(max_mem_alloc_size)
    PRINT(native_vector_width_char)
    PRINT(native_vector_width_double)
    PRINT(native_vector_width_float)
    PRINT(native_vector_width_int)
    PRINT(native_vector_width_long)
    PRINT(native_vector_width_short)
    PRINT(preferred_vector_width_char)
    PRINT(preferred_vector_width_double)
    PRINT(preferred_vector_width_float)
    PRINT(preferred_vector_width_int)
    PRINT(preferred_vector_width_long)
    PRINT(preferred_vector_width_short)
    PRINT(double_fp_config)
    PRINT(half_fp_config)
    PRINT(local_mem_type)
    PRINT(execution_capabilities)
    PRINT(max_work_item_sizes)
    PRINT(queue_properties)
    PRINT(single_fp_config)
    PRINT(type)
    PRINT(platform)
  }
  void PrintPlatformData() {
    for (uint32_t p = 0; p < m_platforms.m_nPlatforms; ++p) {
      std::cout << "Platform Name[" << p
                << "]: " << m_platforms.m_pdata[p].pname << std::endl;
      std::cout << "Platform Vendor[" << p
                << "]: " << m_platforms.m_pdata[p].pvendor << std::endl;
      std::cout << "Platform Version[" << p
                << "]: " << m_platforms.m_pdata[p].pversion << std::endl;
      std::cout << "Platform Profile[" << p
                << "]: " << m_platforms.m_pdata[p].pprofile << std::endl;
      std::cout << "Platform Extension[" << p
                << "]: " << m_platforms.m_pdata[p].pextension << std::endl;
      PrintDeviceData(0, m_platforms.m_ddata[p][0]);
    }
  }
  ~DeviceClass() {
    std::cout << "Destroying device" << std::endl;
    for (uint32_t p = 0; p < m_platforms.m_nPlatforms; ++p) {
      for (uint32_t d = 0; d < m_platforms.m_dev_count[p]; d++) {
#define RELDAT2(x)                                   \
  if (m_platforms.m_ddata[p][d].m_##x.value != NULL) \
    free(m_platforms.m_ddata[p][d].m_##x.value);
        RELDAT2(name)
        RELDAT2(opencl_c_version)
        RELDAT2(extensions)
        RELDAT2(profile)
        RELDAT2(vendor_id)
        RELDAT2(vendor)
        RELDAT2(version)
        clReleaseDevice(m_platforms.m_devs[p][d]);
      }
    }
    if (m_platforms.m_ddata != NULL) delete (m_platforms.m_ddata);
    if (m_platforms.m_platformList != NULL) delete (m_platforms.m_platformList);
    if (m_platforms.m_dev_count != NULL) delete (m_platforms.m_dev_count);
  }

 private:


  cl_int m_err;
  /*
  https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/enums.html#cl_device_info
  */

  void GetDeviceData(cl_platform_id &p, cl_device_id *d,
                     struct device_data &data) {
#define GETDAT3(x)                                      \
  void *tmp##x = printDeviceInfo(d[0], std::get <0> x); \
  std::get <2> x = (typeof(data.m_##x.value))tmp##x; \
  free(tmp##x)
#define GETDAT2(x)                                      \
  void *tmp##x = printDeviceInfo(d[0], data.m_##x.key); \
  data.m_##x.value = (typeof(data.m_##x.value))tmp##x;
#define GETDAT(x)                                                           \
  void *tmp##x = printDeviceInfo(d[0], data.m_##x.key);                     \
  data.m_##x.value = *reinterpret_cast<typeof(data.m_##x.value) *>(tmp##x); \
  free(tmp##x);
    // void *r = printDeviceInfo(d[0], data.m_max_compute_units.key);
    // data.m_max_compute_units.value =
    // *reinterpret_cast<typeof(data.m_max_compute_units.value)*>(r); free(r);
    GETDAT(max_compute_units);
    GETDAT(max_clock_frequency);
    GETDAT(max_read_image_args);
    GETDAT(endian_little);
    GETDAT(address_bits);
    GETDAT(available);
    GETDAT(compiler_available)
    GETDAT(error_correction_support)
    GETDAT(global_mem_cache_size)
    GETDAT(global_mem_cache_type)
    GETDAT(global_mem_cacheline_size)
    GETDAT(global_mem_size)
    GETDAT(host_unified_memory)
    GETDAT(image_support)
    GETDAT(image3d_max_depth)
    GETDAT(image3d_max_height)
    GETDAT(image3d_max_width)
    GETDAT(image2d_max_height)
    GETDAT(image2d_max_width)
    GETDAT(local_mem_size)
    GETDAT(max_parameter_size)
    GETDAT(max_work_group_size)
    GETDAT(profiling_timer_resolution)
    GETDAT(max_constant_args)
    GETDAT(max_samplers)
    GETDAT(max_work_item_dimensions)
    GETDAT(mem_base_addr_align)
    GETDAT(max_constant_buffer_size)
    GETDAT(max_mem_alloc_size)
    GETDAT(native_vector_width_char)
    GETDAT(native_vector_width_double)
    GETDAT(native_vector_width_float)
    GETDAT(native_vector_width_int)
    GETDAT(native_vector_width_long)
    GETDAT(native_vector_width_short)
    GETDAT(preferred_vector_width_char)
    GETDAT(preferred_vector_width_double)
    GETDAT(preferred_vector_width_float)
    GETDAT(preferred_vector_width_int)
    GETDAT(preferred_vector_width_long)
    GETDAT(preferred_vector_width_short)
    GETDAT(double_fp_config)
    GETDAT(half_fp_config)
    GETDAT(local_mem_type)
    GETDAT(execution_capabilities)
    GETDAT2(max_work_item_sizes)
    GETDAT(queue_properties)
    GETDAT(single_fp_config)
    GETDAT(type)
    GETDAT(platform)

    GETDAT2(name)
    GETDAT2(opencl_c_version)
    GETDAT2(extensions)
    GETDAT2(profile)
    GETDAT2(vendor_id)
    GETDAT2(vendor)
    GETDAT2(version)


  }
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

    char *buffer = (char *)malloc(buffer_size);
    m_err = clGetPlatformInfo(platform, param, buffer_size, buffer, NULL);
    CHECKERROR("Call made to clGetPlatformInfo.");

    std::string ret(buffer);
    free(buffer);
    return ret;
  };
  void *printDeviceInfo(cl_device_id &device, cl_device_info param) {
    size_t buffer_size;
    m_err = clGetDeviceInfo(device, param, 0, NULL, &buffer_size);
    CHECKERROR("Call made to clGetDeviceInfo.");

    void *buffer = (void *)malloc(buffer_size);
    m_err = clGetDeviceInfo(device, param, buffer_size, buffer, NULL);
    CHECKERROR("Call made to clGetDeviceInfo.");

    //    std::string ret(buffer);
    //  free(buffer);
    return buffer;
  };
 protected:
  platform_list m_platforms;

};
#endif
