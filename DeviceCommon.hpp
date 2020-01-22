#ifndef OCLCOMMON
#define OCLCOMMON

typedef void (*callbacktype)(cl_event, cl_int, void*);
enum ObjType { Object, Image, Reat };

enum MemType {
  READONLY,
  WRITEONLY,
  READWRITE,
  CPYHOSTPTR,
  ALCHOSTPTR,
  USEHOSTPTR
};



#endif
