#include <string.h>
#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"

int main(char *args)
{

	int argc,len;
	char* level;
	char* buff;

	
	argc = get_argc(args);
	
	if(argc < 2){
		printerr("Key level not specified, keygen <level>\n");
		_exit(0);
	}
	level = get_argv(args,1);
	
	switch((*level)){
		case '1':
		len = 4;
		break;
		
		case '2':
		len = 8;
		break;
		
		case '3':
		len = 16;
		break;
		
		default:
		printerr("Invalid value for level. Value must be in range 1-3\n");
		_exit(0);
			
	}
	
	itoa(len,buff);

	printstr("Generating key with length of ");
	printstr(buff);
	printstr("\n");
	char *buffer;	
	keygen(len,buffer);
	
	_exit(0);
}



