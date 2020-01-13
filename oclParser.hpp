#ifndef OCL_H
#define OCL_H
// Copyright srcmake.com 2018.
// C++ Exception Code
#include <CL/cl.h>
#include <array>
#include <string>
namespace oclParser {

struct platform_data {
  std::string pname;      /* CL_PLATFORM_NAME */
  std::string pversion;   /* CL_PLATFORM_ICD_SUFFIX_KHR or surrogate */
  std::string pvendor;    /* CL_PLATFORM_ICD_SUFFIX_KHR or surrogate */
  std::string pprofile;   /* CL_PLATFORM_ICD_SUFFIX_KHR or surrogate */
  std::string pextension; /* CL_PLATFORM_ICD_SUFFIX_KHR or surrogate */
};



struct dcl_sizet{
  float key;
  std::string name;
  size_t value;
};
struct dcl_uint{
  float key;
  std::string name;
  cl_uint value;
};
struct dcl_ulong{
  float key;
  std::string name;
  cl_ulong value;
};
struct dcl_bool{
  float key;
  std::string name;
  cl_bool value;
};
struct dcl_string{
  float key;
  std::string name;
  std::string value;
};
#define DEFINE_VAL(str, name, val) dcl_##str m_##name {name, #name, val};
struct device_data {
  DEFINE_VAL(uint, CL_DEVICE_ADDRESS_BITS, 0)
  DEFINE_VAL(bool, CL_DEVICE_AVAILABLE, CL_FALSE)
  DEFINE_VAL(bool, CL_DEVICE_COMPILER_AVAILABLE, 0)
  DEFINE_VAL(bool, CL_DEVICE_ENDIAN_LITTLE, 0)
  DEFINE_VAL(bool, CL_DEVICE_ERROR_CORRECTION_SUPPORT, 0)
  DEFINE_VAL(string, CL_DEVICE_EXTENSIONS, 0)
  cl_device_exec_capabilities dexecution_capabilities;

  DEFINE_VAL(ulong, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, 0)
  DEFINE_VAL(ulong, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, 0)
  DEFINE_VAL(ulong, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, 0)
  DEFINE_VAL(ulong, CL_DEVICE_GLOBAL_MEM_SIZE, 0)

  cl_device_fp_config ddouble_fp_config;
  cl_device_fp_config dhalf_fp_config;
  cl_device_local_mem_type dlocal_mem_type;

  DEFINE_VAL(bool, CL_DEVICE_HOST_UNIFIED_MEMORY, 0)
  DEFINE_VAL(bool, CL_DEVICE_IMAGE_SUPPORT, 0)
  DEFINE_VAL(sizet, CL_DEVICE_IMAGE2D_MAX_HEIGHT, 0)
  DEFINE_VAL(sizet, CL_DEVICE_IMAGE2D_MAX_WIDTH, 0)
  DEFINE_VAL(sizet, CL_DEVICE_IMAGE3D_MAX_HEIGHT, 0)
  DEFINE_VAL(sizet, CL_DEVICE_IMAGE3D_MAX_DEPTH, 0)
  DEFINE_VAL(sizet, CL_DEVICE_IMAGE3D_MAX_WIDTH, 0)
  DEFINE_VAL(sizet, CL_DEVICE_LOCAL_MEM_SIZE, 0)
  DEFINE_VAL(sizet, CL_DEVICE_MAX_PARAMETER_SIZE, 0)
  DEFINE_VAL(sizet, CL_DEVICE_MAX_WORK_GROUP_SIZE, 0)
  DEFINE_VAL(sizet, CL_DEVICE_PROFILING_TIMER_RESOLUTION, 0)
  std::array<size_t, 3> dmax_work_item_sizes;

  DEFINE_VAL(uint, CL_DEVICE_MAX_CLOCK_FREQUENCY, 0)
  DEFINE_VAL(uint, CL_DEVICE_MAX_COMPUTE_UNITS, 0)
  DEFINE_VAL(uint, CL_DEVICE_MAX_CONSTANT_ARGS, 0)
  DEFINE_VAL(uint, CL_DEVICE_MAX_READ_IMAGE_ARGS, 0)
  DEFINE_VAL(uint, CL_DEVICE_MAX_SAMPLERS, 0)
  DEFINE_VAL(uint, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 0)
  DEFINE_VAL(uint, CL_DEVICE_MEM_BASE_ADDR_ALIGN, 0)
  DEFINE_VAL(uint, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, 0)
  cl_ulong dmax_constant_buffer_size;
  cl_ulong dmax_mem_alloc_size;
  std::string dname;
  std::string dopencl_version;
  cl_int dnative_vector_width_char;
  cl_int dnative_vector_width_double;
  cl_int dnative_vector_width_float;
  cl_int dnative_vector_width_int;
  cl_int dnative_vector_width_long;
  cl_int dnative_vector_width_short;
  cl_platform_id dplatform;
  cl_int dpreferred_vector_width_char;
  cl_int dpreferred_vector_width_double;
  cl_int dpreferred_vector_width_float;
  cl_int dpreferred_vector_width_int;
  cl_int dpreferred_vector_width_long;
  cl_int dpreferred_vector_width_short;
  std::string dprofile;
  cl_command_queue_properties dqueue_properties;
  cl_device_fp_config dsingle_fp_config;
  cl_device_type dtype;
  std::string dvendor_id;
  std::string dvendor;
  std::string dversion;
  std::string driverversion;
};
struct platform_list {
  /* Number of platforms in the system */
  cl_uint m_nPlatforms = 0;
  /* Total number of devices across all platforms */
  cl_uint m_totalDevices = 0;
  /* Number of devices allocated in all_devs array */
  cl_uint m_maxPlatVersion = 0;
  /* Array of platform IDs */
  cl_platform_id *m_platformList = NULL;
  /* Array of device IDs (across all platforms) */
  cl_device_id **m_devs = NULL;
  /* Array of offsets in all_devs where the devices
   * of each platform begin */
  cl_uint *m_dev_count = NULL;
  /* Array of clinfo-specific platform data */
  struct platform_data *m_pdata = NULL;
  /* Arrau of clinfo-specifici platform checks */
  struct device_data **m_ddata = NULL;
};
}  // namespace oclParser
#endif
