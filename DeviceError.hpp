#ifndef ERROR_H
#define ERROR_H
// Copyright srcmake.com 2018.
// C++ Exception Code
#include <errno.h>
#include <string.h>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST


#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>
using namespace std;

#define BEGIN_DEFINE_LIST                     \
  const char *getoclerrordefs(uint32_t key) { \
    switch (key) {
#define MY_DEFINE(name, value) \
  case value:                  \
    return #name;
#define END_DEFINE_LIST \
  }                     \
  return "Unknown";     \
  }

#define CHECKERROR(x) \
  if (m_err != 0) throw oclException(x, m_err, __FILE__, __LINE__, __func__)
BEGIN_DEFINE_LIST

MY_DEFINE(CL_SUCCESS, 0)
MY_DEFINE(CL_DEVICE_NOT_FOUND, -1)
MY_DEFINE(CL_DEVICE_NOT_AVAILABLE, -2)
MY_DEFINE(CL_COMPILER_NOT_AVAILABLE, -3)
MY_DEFINE(CL_MEM_OBJECT_ALLOCATION_FAILURE, -4)
MY_DEFINE(CL_OUT_OF_RESOURCES, -5)
MY_DEFINE(CL_OUT_OF_HOST_MEMORY, -6)
MY_DEFINE(CL_PROFILING_INFO_NOT_AVAILABLE, -7)
MY_DEFINE(CL_MEM_COPY_OVERLAP, -8)
MY_DEFINE(CL_IMAGE_FORMAT_MISMATCH, -9)
MY_DEFINE(CL_IMAGE_FORMAT_NOT_SUPPORTED, -10)
MY_DEFINE(CL_BUILD_PROGRAM_FAILURE, -11)
MY_DEFINE(CL_MAP_FAILURE, -12)
MY_DEFINE(CL_MISALIGNED_SUB_BUFFER_OFFSET, -13)
MY_DEFINE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST, -14)
MY_DEFINE(CL_COMPILE_PROGRAM_FAILURE, -15)
MY_DEFINE(CL_LINKER_NOT_AVAILABLE, -16)
MY_DEFINE(CL_LINK_PROGRAM_FAILURE, -17)
MY_DEFINE(CL_DEVICE_PARTITION_FAILED, -18)
MY_DEFINE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE, -19)
MY_DEFINE(CL_INVALID_VALUE, -30)
MY_DEFINE(CL_INVALID_DEVICE_TYPE, -31)
MY_DEFINE(CL_INVALID_PLATFORM, -32)
MY_DEFINE(CL_INVALID_DEVICE, -33)
MY_DEFINE(CL_INVALID_CONTEXT, -34)
MY_DEFINE(CL_INVALID_QUEUE_PROPERTIES, -35)
MY_DEFINE(CL_INVALID_COMMAND_QUEUE, -36)
MY_DEFINE(CL_INVALID_HOST_PTR, -37)
MY_DEFINE(CL_INVALID_MEM_OBJECT, -38)
MY_DEFINE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, -39)
MY_DEFINE(CL_INVALID_IMAGE_SIZE, -40)
MY_DEFINE(CL_INVALID_SAMPLER, -41)
MY_DEFINE(CL_INVALID_BINARY, -42)
MY_DEFINE(CL_INVALID_BUILD_OPTIONS, -43)
MY_DEFINE(CL_INVALID_PROGRAM, -44)
MY_DEFINE(CL_INVALID_PROGRAM_EXECUTABLE, -45)
MY_DEFINE(CL_INVALID_KERNEL_NAME, -46)
MY_DEFINE(CL_INVALID_KERNEL_DEFINITION, -47)
MY_DEFINE(CL_INVALID_KERNEL, -48)
MY_DEFINE(CL_INVALID_ARG_INDEX, -49)
MY_DEFINE(CL_INVALID_ARG_VALUE, -50)
MY_DEFINE(CL_INVALID_ARG_SIZE, -51)
MY_DEFINE(CL_INVALID_KERNEL_ARGS, -52)
MY_DEFINE(CL_INVALID_WORK_DIMENSION, -53)
MY_DEFINE(CL_INVALID_WORK_GROUP_SIZE, -54)
MY_DEFINE(CL_INVALID_WORK_ITEM_SIZE, -55)
MY_DEFINE(CL_INVALID_GLOBAL_OFFSET, -56)
MY_DEFINE(CL_INVALID_EVENT_WAIT_LIST, -57)
MY_DEFINE(CL_INVALID_EVENT, -58)
MY_DEFINE(CL_INVALID_OPERATION, -59)
MY_DEFINE(CL_INVALID_GL_OBJECT, -60)
MY_DEFINE(CL_INVALID_BUFFER_SIZE, -61)
MY_DEFINE(CL_INVALID_MIP_LEVEL, -62)
MY_DEFINE(CL_INVALID_PROPERTY, -64)
MY_DEFINE(CL_INVALID_IMAGE_DESCRIPTOR, -65)
MY_DEFINE(CL_INVALID_COMPILER_OPTIONS, -66)
MY_DEFINE(CL_INVALID_LINKER_OPTIONS, -67)
MY_DEFINE(CL_INVALID_DEVICE_PARTITION_COUNT, -68)
MY_DEFINE(CL_INVALID_PIPE_SIZE, -69)
MY_DEFINE(CL_INVALID_DEVICE_QUEUE, -70)
MY_DEFINE(CL_INVALID_SPEC_ID, -71)
MY_DEFINE(CL_MAX_SIZE_RESTRICTION_EXCEEDED, -72)

END_DEFINE_LIST

static std::time_t time_now = std::time(nullptr);

class oclException : public exception {
 public:
  /**
   *                */
  oclException(const string &message, int32_t err_c, const char *fname,
               uint32_t lineno, const char *fxname)
      : userMessage(message) {
    std::stringstream tm;
    tm << std::put_time(std::localtime(&time_now), "%y-%m-%d %OH:%OM:%OS");
    userMessage = tm.str();
    userMessage.append(" [");
    userMessage.append(fname);
    userMessage.append("(");
    userMessage.append(fxname);
    userMessage.append(":");
    userMessage.append(to_string(lineno));
    userMessage.append(")] [");
    userMessage.append(getoclerrordefs(err_c));
    userMessage.append("] \n\t");
    userMessage.append(message);
  //  userMessage.append(strerror(errno));
    userMessage.append("\n Stactrace:\n");
    std::stringstream ss;
    ss << boost::stacktrace::stacktrace();
    userMessage.append(ss.str());
  }
  /**
   **   Provided just to guarantee that no exceptions are thrown.
   **/
  ~oclException() throw(){};

  /**
   **   Get the exception message
   **   @return exception message
   **/
  const char *what() const throw() { return userMessage.c_str(); };

 private:
  string userMessage;  // Exception message
};
#endif
