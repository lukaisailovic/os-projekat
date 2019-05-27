#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"


int main(char *args)
{
	int argc = get_argc(args);
	
	if(argc < 2){
		println("File not specified, decr <file>");
		_exit(1);
	}

	char *file_name = get_argv(args,1);
	
	int fd = open(file_name, O_RDWR);
	
	if(fd == -1){
		println("Could not open file");
		_exit(1);
	}
	decrypt(fd);
	
	_exit(0);
}



