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
extern int testing();
extern void sort();
extern int is_global_key_set();


int buffer_decrypt(char *s, int len){

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
			
			tmp_buff[n++] = s[c_pos + j*keylen];	
		}		
	}
	strcpy(s,tmp_buff);
}

int file_decrypt(struct m_inode *inode){
	int nr, i= 0;
	struct buffer_head *bh;
	while(( nr = bmap(inode, i) )){
		bh = bread(inode->i_dev,nr);
		if(!bh) break;
		buffer_decrypt(bh->b_data,1024);
		bh->b_dirt = 1;
		brelse(bh);
		i++;
	}
}

extern int is_file_encrypted();
extern int decrypt_file();


int sys_decrypt(int fd){
	
	if(!is_global_key_set() && testing == 0){
		printk("Global key not set\n");
		return 0;
	}
	
	printk("Decrypting file...\n");
	struct file *file;
	struct m_inode *inode;
	
	if(fd >= NR_OPEN || !(file=current->filp[fd])){
		return -EINVAL;
	}
	inode = file->f_inode;
	
	if(!is_file_encrypted(inode->i_num)){
		printk("File is not encrypted\n");
		return 0;
	}
	//file_decrypt(inode); // fn
	decrypt_file(inode->i_num); // remove from list of encrypted files
	
	
	
	return 0;
}

