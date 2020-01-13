#ifndef DEVPROGHPP
#define DEVPROGHPP

#include <CL/cl.h>
#include <assert.h>
#include <error.h>
#include <string.h>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class DeviceProgram {
 public:
  DeviceProgram(cl_context& ctx, cl_command_queue& queue, cl_device_id* dev,
                std::string fn)
      : m_context(ctx), m_cmdQueue(queue) {
    m_device = dev;
    m_filename = fn;
    m_initialized = false;
    m_err = CL_SUCCESS;
  }

  ~DeviceProgram() {
    for (size_t i = 0; i < m_kernel.size(); i++) {
      clReleaseKernel(m_kernel.at(i));  // Release kernel.
    }
    clReleaseProgram(m_program);  // Release the program object.
  }

  cl_int CreateProgramWithSource(bool print_code = false) {
    if (convertToString()) {
      if (print_code) std::cout << m_source << std::endl;
    } else {
      std::cerr << "Unable to parse code;";
      return false;
    }
    const char* source = m_source.c_str();
    size_t sourceSize[] = {strlen(source)};
    m_program =
        clCreateProgramWithSource(m_context, 1, &source, sourceSize, &m_err);
    CHECKERROR("Call made to clCreateProgramWithSource.");
    return m_err;
  }

  cl_int BuildProgram(std::string compilerflags = "-Werror") {
    m_err = clBuildProgram(m_program, 1, m_device, compilerflags.c_str(), NULL,
                           NULL);
    CHECKERROR("Call made to clBuildProgram.");
    std::cout << "Printing BuildLog: \n";
    std::cout << GetBuildLog();

    return m_err;
  };

  std::string GetBuildLog() {
    size_t l_sz;
    cl_build_status l_status = CL_BUILD_IN_PROGRESS;
    while (l_status == CL_BUILD_IN_PROGRESS) {
      m_err =
          clGetProgramBuildInfo(m_program, *m_device, CL_PROGRAM_BUILD_STATUS,
                                sizeof(cl_build_status), &l_status, &l_sz);
      CHECKERROR(
          "Call made to clGetProgramBuildInfo(CL_PROGRAM_BUILD_STATUS).");
    }

    if (l_status == CL_BUILD_NONE) {
      std::cerr << "CL_BUILD_NONE" << std::endl;
      return "CL_BUILD_NONE";
    }

    m_err = clGetProgramBuildInfo(m_program, *m_device, CL_PROGRAM_BUILD_LOG, 0,
                                  NULL, &l_sz);
    CHECKERROR("Call made to clGetProgramBuildInfo(LOG_SIZE).");
    char* program_log = (char*)malloc(l_sz + 1);
    program_log[l_sz] = '\0';
    m_err = clGetProgramBuildInfo(m_program, *m_device, CL_PROGRAM_BUILD_LOG,
                                  l_sz + 1, program_log, NULL);
    CHECKERROR("Call made to clGetProgramBuildInfo(ACTUAL_LOG).");

    std::string retString(program_log);
    free(program_log);
    return retString;
  };

  template <class T>
  cl_int AssignArgument(std::string name, T& buffer) {
    m_err = clSetKernelArg(GetKernel(name), 0, sizeof(T), (void*)&buffer);
    CHECKERROR("Call made to clSetKernelArg.");
    return m_err;
  }

  cl_int CreateKernel(std::string name) {
    m_kernelName.push_back(name);
    m_kernel.push_back(clCreateKernel(m_program, name.c_str(), &m_err));
    CHECKERROR("Call made to clCreateKernel.");
    return m_err;
  }

  cl_int RunKernel(std::string name, cl_uint dim_sz,
                   std::array<size_t[1], 3> dim, bool blocking = false) {
    m_err = clEnqueueNDRangeKernel(m_cmdQueue, GetKernel(name), dim_sz, dim[0],
                                   dim[1], dim[2], 0, NULL, NULL);

    if (blocking) WaitTillFinish();
    CHECKERROR("Call made to clEnqueueNDRangeKernel.");
    return m_err;
  }

  void WaitTillFinish() { clFinish(m_cmdQueue); }

  cl_program& GetProgram() { return m_program; }
  cl_kernel& GetKernel(std::string name) {
    return (m_kernel.at(get_kernel_idx(name)));
  }

 private:
  cl_context& m_context;
  cl_device_id* m_device;
  cl_command_queue& m_cmdQueue;

  cl_program m_program;

  std::vector<cl_kernel> m_kernel;
  std::vector<std::string> m_kernelName;

  std::string m_filename;
  std::string m_source;

  cl_int m_err;
  bool m_initialized;

  size_t get_kernel_idx(std::string name) {
    for (size_t i = 0; i < m_kernelName.size(); i++)
      if (m_kernelName.at(i) == name) return i;
    std::stringstream ss;
    ss << "Kernel name not found: " << name;
    throw oclException(ss.str(), -999, __FILE__, __LINE__, __func__);
    throw "";
  }
  /* convert the kernel file into a string */
  bool convertToString() {
    size_t size;
    char* str;
    std::fstream f(m_filename, (std::fstream::in | std::fstream::binary));

    if (f.is_open()) {
      size_t fileSize;
      f.seekg(0, std::fstream::end);
      size = fileSize = (size_t)f.tellg();
      f.seekg(0, std::fstream::beg);
      str = new char[size + 1];
      if (!str) {
        f.close();
        return true;
      }

      f.read(str, fileSize);
      f.close();
      str[size] = '\0';
      m_source = str;
      delete[] str;
      return true;
    }
    std::stringstream ss;
    ss << "Error: failed to open file: " << m_filename;
    throw oclException(ss.str(), -999, __FILE__, __LINE__, __func__);
    return false;
  }
};

#endif
