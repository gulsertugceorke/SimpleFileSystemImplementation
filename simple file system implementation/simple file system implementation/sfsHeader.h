#include <stdio.h>
const int REG_FILE = 0;
const int DIRECTORY = 1;
const int NUMOFINODES = 32; 
const int DATABLOCKSIZE = 512;
const int DIRENTRYSIZE = 32;

//global variable that holds current directory's inodenumber
int curDirInodeNum = 0;

//first part of SFS
struct super_block {
	//32
	int inode_bitmap;
	//10*32
	int data_bitmap[10];
};
//metadata 
struct inode_st{
	int type;
	int size;
//where is the actual data
	int data_block_indices[10];
};
//atomic pair
//store an information of every directory entry 
struct dir_ent{
// for human
	char name[28];
//for computer
	unsigned int inode_no;
};

//initializing superblock
void initialize_sb(struct super_block sb){
	int i = 0;
	sb.inode_bitmap = 0;
	for(i; i<10; i++){
//we initialize 0 because its free so far
		sb.data_bitmap[i] = 0;
	}
	
}

//to get current directory's inodeno
struct inode_st getCurInode(){
	FILE *sfs = fopen("sfs.bin", "r+");
//jump to current directory
	fseek(sfs, sizeof(struct super_block) + (curDirInodeNum*sizeof(struct inode_st)),SEEK_SET);
	struct inode_st curinode;
	fread(&curinode, sizeof(struct inode_st), 1, sfs);
	return curinode;
}
