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


template<typename T>
struct dcl_container {
  cl_uint key;
  std::string name;
  T value;
};

#define CL_DEVICE_type CL_DEVICE_TYPE
#define CL_DEVICE_vendor_id CL_DEVICE_VENDOR_ID
#define CL_DEVICE_max_compute_units CL_DEVICE_MAX_COMPUTE_UNITS
#define CL_DEVICE_max_work_item_dimensions CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
#define CL_DEVICE_max_work_group_size CL_DEVICE_MAX_WORK_GROUP_SIZE
#define CL_DEVICE_max_work_item_sizes CL_DEVICE_MAX_WORK_ITEM_SIZES
#define CL_DEVICE_preferred_vector_width_char \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
#define CL_DEVICE_preferred_vector_width_short \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
#define CL_DEVICE_preferred_vector_width_int \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT
#define CL_DEVICE_preferred_vector_width_long \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG
#define CL_DEVICE_preferred_vector_width_float \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
#define CL_DEVICE_preferred_vector_width_double \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE
#define CL_DEVICE_max_clock_frequency CL_DEVICE_MAX_CLOCK_FREQUENCY
#define CL_DEVICE_address_bits CL_DEVICE_ADDRESS_BITS
#define CL_DEVICE_max_read_image_args CL_DEVICE_MAX_READ_IMAGE_ARGS
#define CL_DEVICE_max_write_image_args CL_DEVICE_MAX_WRITE_IMAGE_ARGS
#define CL_DEVICE_max_mem_alloc_size CL_DEVICE_MAX_MEM_ALLOC_SIZE
#define CL_DEVICE_image2d_max_width CL_DEVICE_IMAGE2D_MAX_WIDTH
#define CL_DEVICE_image2d_max_height CL_DEVICE_IMAGE2D_MAX_HEIGHT
#define CL_DEVICE_image3d_max_width CL_DEVICE_IMAGE3D_MAX_WIDTH
#define CL_DEVICE_image3d_max_height CL_DEVICE_IMAGE3D_MAX_HEIGHT
#define CL_DEVICE_image3d_max_depth CL_DEVICE_IMAGE3D_MAX_DEPTH
#define CL_DEVICE_image_support CL_DEVICE_IMAGE_SUPPORT
#define CL_DEVICE_max_parameter_size CL_DEVICE_MAX_PARAMETER_SIZE
#define CL_DEVICE_max_samplers CL_DEVICE_MAX_SAMPLERS
#define CL_DEVICE_mem_base_addr_align CL_DEVICE_MEM_BASE_ADDR_ALIGN
#define CL_DEVICE_min_data_type_align_size CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
#define CL_DEVICE_single_fp_config CL_DEVICE_SINGLE_FP_CONFIG
#define CL_DEVICE_global_mem_cache_type CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
#define CL_DEVICE_global_mem_cacheline_size CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE
#define CL_DEVICE_global_mem_cache_size CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
#define CL_DEVICE_global_mem_size CL_DEVICE_GLOBAL_MEM_SIZE
#define CL_DEVICE_max_constant_buffer_size CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
#define CL_DEVICE_max_constant_args CL_DEVICE_MAX_CONSTANT_ARGS
#define CL_DEVICE_local_mem_type CL_DEVICE_LOCAL_MEM_TYPE
#define CL_DEVICE_local_mem_size CL_DEVICE_LOCAL_MEM_SIZE
#define CL_DEVICE_error_correction_support CL_DEVICE_ERROR_CORRECTION_SUPPORT
#define CL_DEVICE_profiling_timer_resolution \
  CL_DEVICE_PROFILING_TIMER_RESOLUTION
#define CL_DEVICE_endian_little CL_DEVICE_ENDIAN_LITTLE
#define CL_DEVICE_available CL_DEVICE_AVAILABLE
#define CL_DEVICE_compiler_available CL_DEVICE_COMPILER_AVAILABLE
#define CL_DEVICE_execution_capabilities CL_DEVICE_EXECUTION_CAPABILITIES
#define CL_DEVICE_queue_properties CL_DEVICE_QUEUE_PROPERTIES /* deprecated */
#define CL_DEVICE_queue_on_host_properties CL_DEVICE_QUEUE_ON_HOST_PROPERTIES
#define CL_DEVICE_name CL_DEVICE_NAME
#define CL_DEVICE_vendor CL_DEVICE_VENDOR
#define CL_DRIVER_version CL_DRIVER_VERSION
#define CL_DEVICE_profile CL_DEVICE_PROFILE
#define CL_DEVICE_version CL_DEVICE_VERSION
#define CL_DEVICE_extensions CL_DEVICE_EXTENSIONS
#define CL_DEVICE_platform CL_DEVICE_PLATFORM
#define CL_DEVICE_double_fp_config CL_DEVICE_DOUBLE_FP_CONFIG
#define CL_DEVICE_half_fp_config CL_DEVICE_HALF_FP_CONFIG
#define CL_DEVICE_preferred_vector_width_half \
  CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
#define CL_DEVICE_host_unified_memory \
  CL_DEVICE_HOST_UNIFIED_MEMORY /* deprecated */
#define CL_DEVICE_native_vector_width_char CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR
#define CL_DEVICE_native_vector_width_short CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT
#define CL_DEVICE_native_vector_width_int CL_DEVICE_NATIVE_VECTOR_WIDTH_INT
#define CL_DEVICE_native_vector_width_long CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG
#define CL_DEVICE_native_vector_width_float CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT
#define CL_DEVICE_native_vector_width_double \
  CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE
#define CL_DEVICE_native_vector_width_half CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF
#define CL_DEVICE_opencl_c_version CL_DEVICE_OPENCL_C_VERSION
#define CL_DEVICE_linker_available CL_DEVICE_LINKER_AVAILABLE
#define CL_DEVICE_built_in_kernels CL_DEVICE_BUILT_IN_KERNELS
#define CL_DEVICE_image_max_buffer_size CL_DEVICE_IMAGE_MAX_BUFFER_SIZE
#define CL_DEVICE_image_max_array_size CL_DEVICE_IMAGE_MAX_ARRAY_SIZE
#define CL_DEVICE_parent_device CL_DEVICE_PARENT_DEVICE
#define CL_DEVICE_partition_max_sub_devices CL_DEVICE_PARTITION_MAX_SUB_DEVICES
#define CL_DEVICE_partition_properties CL_DEVICE_PARTITION_PROPERTIES
#define CL_DEVICE_partition_affinity_domain CL_DEVICE_PARTITION_AFFINITY_DOMAIN
#define CL_DEVICE_partition_type CL_DEVICE_PARTITION_TYPE
#define CL_DEVICE_reference_count CL_DEVICE_REFERENCE_COUNT
#define CL_DEVICE_preferred_interop_user_sync \
  CL_DEVICE_PREFERRED_INTEROP_USER_SYNC
#define CL_DEVICE_printf_buffer_size CL_DEVICE_PRINTF_BUFFER_SIZE
#define CL_DEVICE_image_pitch_alignment CL_DEVICE_IMAGE_PITCH_ALIGNMENT
#define CL_DEVICE_image_base_address_alignment \
  CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT
#define CL_DEVICE_max_read_write_image_args CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS
#define CL_DEVICE_max_global_variable_size CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE
#define CL_DEVICE_queue_on_device_properties \
  CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES
#define CL_DEVICE_queue_on_device_preferred_size \
  CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE
#define CL_DEVICE_queue_on_device_max_size CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE
#define CL_DEVICE_max_on_device_queues CL_DEVICE_MAX_ON_DEVICE_QUEUES
#define CL_DEVICE_max_on_device_events CL_DEVICE_MAX_ON_DEVICE_EVENTS
#define CL_DEVICE_svm_capabilities CL_DEVICE_SVM_CAPABILITIES
#define CL_DEVICE_global_variable_preferred_total_size \
  CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE
#define CL_DEVICE_max_pipe_args CL_DEVICE_MAX_PIPE_ARGS
#define CL_DEVICE_pipe_max_active_reservations \
  CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS
#define CL_DEVICE_pipe_max_packet_size CL_DEVICE_PIPE_MAX_PACKET_SIZE
#define CL_DEVICE_preferred_platform_atomic_alignment \
  CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT
#define CL_DEVICE_preferred_global_atomic_alignment \
  CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT
#define CL_DEVICE_preferred_local_atomic_alignment \
  CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT
#define CL_DEVICE_il_version CL_DEVICE_IL_VERSION
#define CL_DEVICE_max_num_sub_groups CL_DEVICE_MAX_NUM_SUB_GROUPS
#define CL_DEVICE_sub_group_independent_forward_progress \
  CL_DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS

#define DEFINE_VAL2(type, name, val) \
  dcl_container<type> m_##name{CL_DEVICE_##name, #name, val};
struct device_data {
  DEFINE_VAL2(uint, address_bits, 0)
  DEFINE_VAL2(bool, available, CL_FALSE)
  DEFINE_VAL2(bool, compiler_available, 0)
  DEFINE_VAL2(bool, endian_little, 0)
  DEFINE_VAL2(bool, error_correction_support, 0)

  DEFINE_VAL2(ulong, global_mem_cache_size, 0)
  DEFINE_VAL2(ulong, global_mem_cache_type, 0)
  DEFINE_VAL2(ulong, global_mem_cacheline_size, 0)
  DEFINE_VAL2(ulong, global_mem_size, 0)

  DEFINE_VAL2(cl_device_fp_config, double_fp_config, 0)
  DEFINE_VAL2(cl_device_fp_config, half_fp_config, 0)
  DEFINE_VAL2(cl_device_local_mem_type, local_mem_type, 0)
  DEFINE_VAL2(cl_device_exec_capabilities, execution_capabilities, 0)
  DEFINE_VAL2(size_t*, max_work_item_sizes, 0)
  DEFINE_VAL2(cl_command_queue_properties, queue_properties, 0)
  DEFINE_VAL2(cl_device_fp_config, single_fp_config, 0)
  DEFINE_VAL2(cl_device_type, type, 0)
  DEFINE_VAL2(cl_platform_id, platform, 0)

  DEFINE_VAL2(bool, host_unified_memory, 0)
  DEFINE_VAL2(bool, image_support, 0)
  DEFINE_VAL2(size_t, image2d_max_height, 0)
  DEFINE_VAL2(size_t, image2d_max_width, 0)
  DEFINE_VAL2(size_t, image3d_max_height, 0)
  DEFINE_VAL2(size_t, image3d_max_depth, 0)
  DEFINE_VAL2(size_t, image3d_max_width, 0)
  DEFINE_VAL2(size_t, local_mem_size, 0)
  DEFINE_VAL2(size_t, max_parameter_size, 0)
  DEFINE_VAL2(size_t, max_work_group_size, 0)
  DEFINE_VAL2(size_t, profiling_timer_resolution, 0)

  DEFINE_VAL2(uint, max_clock_frequency, 0)
  DEFINE_VAL2(uint, max_compute_units, 0)
  DEFINE_VAL2(uint, max_constant_args, 0)
  DEFINE_VAL2(uint, max_read_image_args, 0)
  DEFINE_VAL2(uint, max_samplers, 0)
  DEFINE_VAL2(uint, max_work_item_dimensions, 0)
  DEFINE_VAL2(uint, mem_base_addr_align, 0)
  DEFINE_VAL2(uint, min_data_type_align_size, 0)
  DEFINE_VAL2(ulong, max_constant_buffer_size, 0)
  DEFINE_VAL2(ulong, max_mem_alloc_size, 0)
  DEFINE_VAL2(uint, native_vector_width_char, 0)
  DEFINE_VAL2(uint, native_vector_width_double, 0)
  DEFINE_VAL2(uint, native_vector_width_float, 0)
  DEFINE_VAL2(uint, native_vector_width_int, 0)
  DEFINE_VAL2(uint, native_vector_width_long, 0)
  DEFINE_VAL2(uint, native_vector_width_short, 0)
  DEFINE_VAL2(uint, preferred_vector_width_char, 0)
  DEFINE_VAL2(uint, preferred_vector_width_double, 0)
  DEFINE_VAL2(uint, preferred_vector_width_float, 0)
  DEFINE_VAL2(uint, preferred_vector_width_int, 0)
  DEFINE_VAL2(uint, preferred_vector_width_long, 0)
  DEFINE_VAL2(uint, preferred_vector_width_short, 0)
  DEFINE_VAL2(char*, name, NULL)
  DEFINE_VAL2(char*, opencl_c_version, NULL)
  DEFINE_VAL2(char*, extensions, NULL)
  DEFINE_VAL2(char*, profile, NULL)
  DEFINE_VAL2(char*, vendor_id, NULL)
  DEFINE_VAL2(char*, vendor, NULL)
  DEFINE_VAL2(char*, version, NULL)
  dcl_container<char*> m_driver_version{CL_DRIVER_VERSION, "CL_DRIVER_VERSION", NULL};
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
