#include <errno.h>
#include <string.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

/* Utils */



int secure_input_active = 0;


static void __reverse(char *buf, int len)
{
	int i, j;
	char c;
	for(i = 0, j = len - 1; i < j; ++i, --j)
	{
		c = buf[i];
		buf[i] = buf[j];
		buf[j] = c;
	}
}

int itoa(int n, char *buf)
{
	int i = 0, sign;
	if((sign = n) < 0)
		n = -n;
	do
	{
		buf[i++] = n % 10 + '0';
	} while((n /= 10) > 0);

	if(sign < 0)
		buf[i++] = '-';
	buf[i] = '\0';
	__reverse(buf, i);
	return i;
}

void printint(int n){
	char buff[1024];
	itoa(n,buff);
	printk(buff);
		
}

int r;
int generate_random(int range_start, int range_end, int offset){
	/* za random */
	__asm__ __volatile__ (
		"rdtsc;"
		: "=a" (r)
		:
		: "%edx"
	);
	
	__asm__ __volatile__ ( 
		"movl (r), %%eax;"
		"imull $7621, %%eax;"
		"addl $1, %%eax;"
		"movl $32768, %%ebx;"
		"xorl %%edx, %%edx;"
		"idivl %%ebx;"
		"movl %%edx, (r);"
		:
		:
		: "%eax", "%edx","%ebx","memory"
	);
	return ((r + offset) % (range_end - range_start)) + range_start;
}

int is_random_valid(const char *key,int key_length, int rand){
	int i;
	int valid = 1;
	for(i = 0; i < key_length; i++){
		if((*(key+i)) == rand){
			valid = 0;
			break;	
		}
	}
	return valid;
}

int sys_keygen(int length, const char *buffer){
	char key[17];
	int i, rand;
	
	for(i = 0; i < length; i++){						
		while(1){
			rand = generate_random(33,126,i);		
			if(is_random_valid(key,length,rand)){				
				key[i] = rand;			
				break;
			} 		
		}		
	}
	key[i+1] = 0;
	printk(key);
	printk("\n");
	return 0;
}

char global_key[1024];

int sys_keyset(int keylen,const char *key){
	int i ;
	for (i = 0; i < 1024;i++){
		global_key[i] = get_fs_byte(key++);
	}
	//strcpy(&global_key,key);
	//strcpy(&global_key,"projekat");
	sys_keyshow();
	return 0;
	
}

int sys_keyclear(){
	strcpy(&global_key,"");
	printk("Global key cleared\n");
	return 0;
}

int sys_keyshow(){
	if(strlen(global_key) == 0){
		printk("Global key is not set\n");
	} else {
		printk("Global key is set to: ");
		printk(global_key);
		printk("\n");
	}
	
	return 0;
}

int sys_secureinput(int active){
	secure_input_active = active;
}


int is_global_key_set(){
	return strlen(global_key) > 0;
}





