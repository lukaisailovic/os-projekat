#include <errno.h>
#include <string.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

/* Utils */

extern char global_key[1024];
extern void printint();

extern int sys_decrypt();
extern int testing;
extern int is_global_key_set();


unsigned short encrypted_files[1024];
int current_encrypted_files = 0;

int is_file_encrypted(unsigned short i_uid){
	int i;
	int r = 0;
	for(i = 0; i < current_encrypted_files;i++){
		if(encrypted_files[i] == i_uid){
			r = 1;
			break;
		}
	}
	return r;
}

int decrypt_file(unsigned short i_uid){
	int i;
	for(i = 0; i < current_encrypted_files;i++){
		if(encrypted_files[i] == i_uid){
			encrypted_files[i] = NULL;
			break;
		}
	}
	return 0;
}

void sort(const char* buff){
   	char temp;
   	char string[1024];
   	strcpy(string,buff);	
   	int i, j;
   	int n = strlen(string);
    for (i = 0; i < n-1; i++) {
      	for (j = i+1; j < n; j++) {
         	if (string[i] > string[j]) {
		        temp = string[i];
		        string[i] = string[j];
		        string[j] = temp;
         	}
      	}
   	}
   	strcpy(buff,string);
}


int sys_encr(int fd){
	sys_decrypt(fd);
	return 0;
}


int buffer_encrypt(char *s, int len){

	char key[1024];
	char keysort[1024];
	char tmp_buff[1024];
	
	strcpy(tmp_buff,s); //copy buff to tmp buff;
	
	if(testing){
		strcpy(key,"projekat");
	} else {
		strcpy(key,global_key);	//get global key
	}		
	strcpy(keysort,key);	
	sort(keysort);	// get sorted key
		
	int keylen = strlen(key);
	int row_count = len/keylen; // number of rows 	

	/* Get current position of the first char in sorted str */
	// c_pos - current char position
	// c_char_offset - already used chars 
	int c_char_offset = 0;
	char *e;
	int c_pos;

	int i,j,n,row;
	row = 0;
	n = 0;
	for(i = 0; i < keylen; i++){	// loop - keylen (8 times) where i is c_char_offset
		
		e = strchr(key, keysort[i]);
		c_pos = (int)(e - key); // start position of the row
			
		for(j = 0; j < row_count; j++){ // loop - row_count (128 times) where j is row
			//char c = (char)s[c_pos + j*keylen];
			//printk("%c",c);			
			//tmp_buff[n++] = s[c_pos + j*keylen]; // c_pos - first char in column	
			s[n++] = tmp_buff[c_pos + j*keylen];
		}		
	}
	/*
	//strcpy(s,tmp_buff);
	for(i=0;i< len; i++){
		s[i] = tmp_buff[i];
	}
	*/
	
	if(testing){
		for(i=0;i< len; i++){
			char c = (char)s[i];
			printk("%c",c);		
		}
		printk("\nEncryption done\n");
	}	
	return 0;
}

int file_encrypt(struct m_inode *inode){
	int nr,j, i= 0;
	struct buffer_head *bh;
	while(( nr = bmap(inode, i) )){
		bh = bread(inode->i_dev,nr);
		if(!bh) break;
		
		char * p = bh->b_data;
		
		char * buf = bh->b_data;
		
		
				
		buffer_encrypt(buf,1024);		
				
		j = 0;	
		while (j < 1024){
			
			*(p++) = buf++;
			j++;	
		}	
			
		bh->b_dirt = 1;
		brelse(bh);
		i++;
	}
}

int sys_encrypt(int fd){

	if(!is_global_key_set() && testing == 0){
		printk("Global key not set\n");
		return 0;
	}

	printk("Encrypting file\n");
	struct file *file;
	struct m_inode *inode;
	
	if(fd >= NR_OPEN || !(file=current->filp[fd])){
		return -EINVAL;
	}
	inode = file->f_inode;
	file_encrypt(inode);
	encrypted_files[current_encrypted_files++] = inode->i_num;
	return 0;
}

