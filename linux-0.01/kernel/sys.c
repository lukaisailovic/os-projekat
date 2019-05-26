#include <errno.h>
#include <string.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

int sys_ftime()
{
	return -ENOSYS;
}

int sys_mknod()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_mount()
{
	return -ENOSYS;
}

int sys_umount()
{
	return -ENOSYS;
}

int sys_ustat(int dev,struct ustat * ubuf)
{
	return -1;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setgid(int gid)
{
	if (current->euid && current->uid)
		if (current->gid==gid || current->sgid==gid)
			current->egid=gid;
		else
			return -EPERM;
	else
		current->gid=current->egid=gid;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

int sys_setuid(int uid)
{
	if (current->euid && current->uid)
		if (uid==current->uid || current->suid==current->uid)
			current->euid=uid;
		else
			return -EPERM;
	else
		current->euid=current->uid=uid;
	return 0;
}

int sys_stime(long * tptr)
{
	if (current->euid && current->uid)
		return -1;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (!tbuf)
		return jiffies;
	verify_area(tbuf,sizeof *tbuf);
	put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
	put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
	put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
	put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->uid && current->euid)
		return -EPERM;
	if (current->leader)
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_oldolduname(void* v)
{
	printk("calling obsolete system call oldolduname\n");
	return -ENOSYS;
//	return (0);
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux 0.01-3.x","nodename","release ","3.x","i386"
	};
	int i;

	if (!name) return -1;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return (0);
}

int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

int sys_null(int nr)
{
	static int prev_nr=-2;
	if (nr==174 || nr==175) return -ENOSYS;

	if (prev_nr!=nr) 
	{
		prev_nr=nr;
//		printk("system call num %d not available\n",nr);
	}
	return -ENOSYS;
}

/* Utils */


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

/* OS2019 */
extern long user_key_ptr, user_shift_ptr, user_alt_ptr;
extern int selected_layout, user_key_map_size;



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
	

	
	
	//strcpy(&global_key,key);
	strcpy(&global_key,"projekat");
	printk("Global key is set to: ");
	printk(global_key);
	printk("\n");
	return 0;
	
}


int sys_change_user_layout(const char *layout, int map)
{
	int i;
	char *maps[] = {(char*)user_key_ptr, (char*)user_shift_ptr, (char*)user_alt_ptr};
	if(map < 0 || map > 2)
	{
		return -1;
	}
	char *mp = maps[map], c;
	for(i = 0; i < user_key_map_size; ++i)
	{
		c = get_fs_byte(layout + i);
		*mp++ = c;
	}
	selected_layout = 2;
	return 0;
}





