#include <string.h>
#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"



int is_power_of_two(int n)
{
  if (n == 0)
    return 0;
  while (n != 1)
  {
      if (n%2 != 0)
         return 0;
      n = n/2;
  }
  return 1;
}


int main(char *args)
{

	int argc,len;
	char* key;
	char* buff;

	
	argc = get_argc(args);
	
	if(argc < 2){
		printerr("Key not specified, keygen <key>\n");
		_exit(0);
	}
	key = get_argv(args,1);
	int keylen = strlen(key);
	if(keylen > 1024){
		printerr("Key length cannot be greater than 1024\n");
		_exit(0);
	}
	
	if(!is_power_of_two(keylen)){
		printerr("Key not valid, key length must be power of 2\n");
		_exit(0);
	}
	/*	
	println("Key passed:  ");
	println(key);
	println("=======");
	*/
	keyset(strlen(key),key);
	
	
	
	_exit(0);
}



