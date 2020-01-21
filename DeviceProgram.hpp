#ifndef DEVPROGHPP
#define DEVPROGHPP

#include <CL/cl.h>
#include <assert.h>
#include <string.h>
#include <DeviceError.hpp>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

typedef void (*callbacktype)(cl_event, cl_int, void*);

class DeviceProgram {
 public:
  DeviceProgram(cl_context& ctx, cl_command_queue& queue, cl_device_id* dev,
                std::string fn, bool profiling = false,
                bool hostnotification = false)
      : m_context(ctx), m_cmdQueue(queue) {
    m_device = dev;
    m_filename = fn;
    m_profiling = profiling;
    m_hostnotification = hostnotification;
    m_err = CL_SUCCESS;
  }

  ~DeviceProgram() {
    for (size_t i = 0; i < m_kernel.size(); i++) {
      clReleaseKernel(m_kernel.at(i).second);  // Release kernel.
    }
    clReleaseProgram(m_program);  // Release the program object.
  }

  cl_int CreateProgramWithSource(bool print_code = false) {
    if (convertToString()) {
      if (print_code) {
        std::cout << "\n\n"
                  << "Source code for kernel: " << m_filename << std::endl;
        std::cout << m_source << std::endl << std::endl;
      }
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
    if (m_err != CL_BUILD_PROGRAM_FAILURE)
      CHECKERROR("Call made to clBuildProgram.");

    std::cout << "BUildStatus: " << getoclerrordefs(m_err) << std::endl;
    if (m_err == CL_BUILD_PROGRAM_FAILURE)
      std::cout << "Printing BuildLog: \n" << GetBuildLog() << std::endl;
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

  cl_int AssignLocalArgument(std::string name, cl_uint idx, size_t sz = 1) {
    m_err = clSetKernelArg(GetKernel(name), idx, sz, NULL);
    CHECKERROR("Call made to clSetKernelArg.");
    return m_err;
  }
  template <class T>
  cl_int AssignArgument(std::string name, cl_uint idx, T& buffer,
                        size_t sz = 1) {
    m_err =
        clSetKernelArg(GetKernel(name), idx, sizeof(T) * sz, (void*)&buffer);
    CHECKERROR("Call made to clSetKernelArg.");
    return m_err;
  }

  cl_int CreateKernel(std::string name) {
    m_kernel.push_back(
        std::make_pair(name, clCreateKernel(m_program, name.c_str(), &m_err)));
    CHECKERROR("Call made to clCreateKernel.");
    return m_err;
  }

  cl_int RunKernel(std::string name, cl_uint dim_sz, std::array<size_t*, 3> dim,
                   cl_event& ev, callbacktype* func = NULL,
                   bool blocking = false, cl_uint n_ev = 0,
                   cl_event* w_ev = NULL, const char* msg = NULL) {
    m_err = clEnqueueNDRangeKernel(m_cmdQueue, GetKernel(name), dim_sz, dim[0],
                                   dim[1], dim[2], n_ev, w_ev, &ev);

    CHECKERROR("Call made to clEnqueueNDRangeKernel.");
    if (m_hostnotification == true) {
      if (func != NULL) {
        m_err = clSetEventCallback(ev, CL_COMPLETE, *func, (void*)msg);
        CHECKERROR("Call made to clSetEventCallback.");
      } else {
        m_err =
            clSetEventCallback(ev, CL_COMPLETE, &default_callback, (void*)msg);
        CHECKERROR("Call made to clSetEventCallback.");
      }
    } else if (m_profiling == true) {
      m_err =
          clSetEventCallback(ev, CL_COMPLETE, &profiling_callback, (void*)msg);
      CHECKERROR("Call made to clSetEventCallback.");
    }

    if (blocking) WaitTillFinish();
    return m_err;
  }

  void WaitTillFinish() { clFinish(m_cmdQueue); }

  cl_program& GetProgram() { return m_program; }
  cl_kernel& GetKernel(std::string name) {
    for (size_t i = 0; i < m_kernel.size(); i++) {
      if (m_kernel.at(i).first == name) {
        return m_kernel.at(i).second;
      }
    }
    m_err = -9999;
    name = "Unable to find buffer: " + name;
    CHECKERROR(name);
    return m_kernel.at(0).second;
  }

 private:
  cl_context& m_context;
  cl_device_id* m_device;
  cl_command_queue& m_cmdQueue;

  cl_program m_program;

  std::vector<std::pair<std::string, cl_kernel>> m_kernel;

  std::string m_filename;
  std::string m_source;

  cl_int m_err;
  bool m_initialized;
  bool m_profiling;
  bool m_hostnotification;

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
    printf("Average execution time: %lu\n", end - start);

    //  if (e != NULL) delete e;
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
