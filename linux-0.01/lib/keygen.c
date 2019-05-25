#define __LIBRARY__
#include <unistd.h>

_syscall2(int,keygen,int,length,const char*,buffer)
