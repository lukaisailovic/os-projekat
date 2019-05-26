#define __LIBRARY__
#include <unistd.h>

_syscall2(int,keyset,int,length,const char*,buffer)
