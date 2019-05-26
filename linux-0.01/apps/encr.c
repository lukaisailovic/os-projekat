#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"


int main(char *args)
{
	int argc = get_argc(args);
	
	if(argc < 2){
		println("File not specified, encr <file>");
		_exit(1);
	}

	char *file_name = get_argv(args,1);
	println("File name:");
	println(file_name);
	
	
	int fd = open(file_name, O_RDWR);
	
	if(fd == -1){
		println("Could not open file");
		_exit(1);
	}
	encrypt(fd);
	
	_exit(0);
}



