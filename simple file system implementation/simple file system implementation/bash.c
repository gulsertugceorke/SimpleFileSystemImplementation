#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sfsHeader.h"

void mkfile();

void ls();

void mkdir();

void cd();

void lsrec();

int getFreeInodeNumber(struct super_block sb);
int getFreeDataBlockNumber(struct super_block sb);

void updateINodeNo(struct super_block *sb, int p, int b);
void updateDataBlockNo(struct super_block *sb, int p, int b, int index);
void updateSuperBlock(struct super_block *sb);

struct super_block sb;
struct inode_st root;


int main(){

//scanning command	
char command [32];
	while(1){
		printf("> ");
		scanf("%s", command);
		
		if(strcmp(command, "ls") == 0){
			ls();
		}
		if(strcmp(command, "mkfile") == 0){
			mkfile();
		}
		if(strcmp(command, "mkdir") == 0){
			mkdir();
		}
		if(strcmp(command, "cd") == 0){
			cd();
		}
		if(strcmp(command, "lsrec") == 0){
			lsrec(0);
		}
		else {
			
		}
		
	}
	
	return 0;
}


void cd() {
	int i;
	char dirName[32];
	scanf("%s", dirName);
//boolean value to check file existing
	bool exists = false;
//temporary entry to read directory
	struct dir_ent entry;
	FILE *sfs = fopen("sfs.bin", "r");
	struct inode_st curinode = getCurInode();    
//calculate how much entries in the current directory	        
	int numOfEntries = curinode.size/sizeof(struct dir_ent); 
	fseek(sfs, sizeof(sb)+NUMOFINODES*sizeof(struct inode_st), SEEK_SET);
	for(i = 0; i < numOfEntries; i++){
		fread(&entry, sizeof(struct dir_ent), 1, sfs);
		if(strcmp(entry.name, dirName) == 0){
//if exists returns true
			exists = true;
//to read inodeno directory which will be changed
		fseek(sfs, sizeof(struct super_block)+(entry.inode_no*sizeof(struct inode_st)), SEEK_SET); 
		struct inode_st inode;
		fread(&inode, sizeof(struct inode_st),1,sfs);
		if(inode.type == DIRECTORY){
//curdirInodeNumber is updated
		curDirInodeNum = entry.inode_no;
		printf("curdirinodeno: %d\n",curDirInodeNum);
		}
		
		else {
		printf("this is not a directory\n");
			}
		break;
		}
	}
		
		if(!exists){
			printf("there is no such directory or file\n");
		}
	
	fclose(sfs);
}
		
void mkfile(){
	
    int i;
	char fileName[32];
	char bio[] = "tugce beyza and serdar are bffxd";
	scanf("%s", fileName);
	
	struct inode_st newFile;
	newFile.type = REG_FILE;
	newFile.size = sizeof(bio);
	
	FILE *sfs = fopen("sfs.bin", "r+");
	
	int  newInode = getFreeInodeNumber(sb);
	printf("inodeno: %d\n",newInode);
	
	int datablock = getFreeDataBlockNumber(sb);
	
	printf("datablock: %d\n",datablock);
	
	for(i=0; i<10; i++){
		newFile.data_block_indices[i] = datablock; 	
		
	}

//0-9
	int dataBlockIndex = datablock/32;
//0-31
	int position = datablock %32;
	printf("data blockindex: %d\n",dataBlockIndex);
	
	updateINodeNo(&sb, newInode, 1);
	updateDataBlockNo(&sb, position, 1, dataBlockIndex);
	//5
	updateSuperBlock(&sb);
	
	struct inode_st curinode = getCurInode();
	curinode.size += sizeof(struct dir_ent);
	
	
	fseek(sfs, sizeof(struct super_block) + (curDirInodeNum*sizeof(struct inode_st)), SEEK_SET);
	fwrite(&curinode, sizeof(struct inode_st), 1, sfs);
	
	
	fseek(sfs, sizeof(struct super_block)+newInode*sizeof(struct inode_st), SEEK_SET);	
	fwrite(&newFile, sizeof(struct inode_st), 1, sfs);
	
	fseek(sfs, sizeof(struct super_block)+(32*sizeof(struct inode_st))+(newFile.data_block_indices[0]*DATABLOCKSIZE),SEEK_SET);
	fwrite(&bio, sizeof(struct inode_st), 1, sfs);

	struct dir_ent newFileEntry;
	strcpy(newFileEntry.name, fileName);
	newFileEntry.inode_no = newInode;
	
	
	fseek(sfs, sizeof(struct super_block)+(32*sizeof(struct inode_st))+(curinode.data_block_indices[0]*DATABLOCKSIZE) + (curinode.size-DIRENTRYSIZE),SEEK_SET);
	
	printf("new file size: %d\n",newFile.size);
	
	fwrite(&newFileEntry, sizeof(struct dir_ent), 1, sfs);
	
	fclose(sfs);
		
} 

void lsrec(int level){

	FILE *sfs = fopen("sfs.bin", "r+");
	struct inode_st curinode = getCurInode(); 
//to check directory or not	
	if(curinode.type == DIRECTORY){
	int numOfEntries = curinode.size/sizeof(struct dir_ent);
	fseek(sfs, sizeof(struct super_block)+NUMOFINODES*sizeof(struct inode_st)+(curinode.data_block_indices[0]*DATABLOCKSIZE),SEEK_SET);
	struct dir_ent ents[numOfEntries];
	int i;
	
	for(i=0; i<numOfEntries; i++){
		fread(&ents[i], sizeof(struct dir_ent),1,sfs);  
	int j;	
	for(j=0; j<level; j++){
		printf("   ");
		} 	
	printf( "%s\n", ents[i].name);
//to read another directoryies in the current directory
	 if(strcmp(ents[i].name, ".") != 0 && strcmp(ents[i].name, "..") != 0){
		curDirInodeNum = ents[i].inode_no;
		level++;
		lsrec(level);
		level--;
		
			}	
		}
	}

}



void ls (){
	int i;
	struct dir_ent entry;
	FILE *sfs = fopen("sfs.bin", "r");
	struct inode_st curinode = getCurInode();  
	
//	printf("gfd: %d\n",curinode.size);
	        
	int numOfEntries = curinode.size/sizeof(struct dir_ent); 
/*	fseek(sfs, sizeof(sb)+32*sizeof(struct inode_st), SEEK_SET); */
	fseek(sfs, sizeof(struct super_block)+(NUMOFINODES*sizeof(struct inode_st))+(curinode.data_block_indices[0]*DATABLOCKSIZE),SEEK_SET);
	for(i = 0; i < numOfEntries; i++){
		fread(&entry, sizeof(struct dir_ent), 1, sfs);
		printf( "%s\n", entry.name);
	}
	fclose(sfs);

}


/*We first create a mask that has set bit only 
at given position using bit wise shift.
      mask = 1 << position

Then to change value of bit to b, we first
make it 0 using below operation
      value & ~mask

After changing it 0, we change it to b by
doing or of above expression with following
(b << p) & mask, i.e., we return
      (n & ~mask) | ((b << p) & mask) */
      
void updateINodeNo(struct super_block *sb, int position, int bitmap){
	
	int mask = 1 << position;
	sb->inode_bitmap = (sb->inode_bitmap & ~mask) | ((bitmap << position) & mask);
	
}	
void updateDataBlockNo(struct super_block *sb, int position, int bitmap, int index){
	int mask = 1 << position;
	sb->data_bitmap[index] = (sb->data_bitmap[index] & ~mask) | ((bitmap << position) & mask);

}


void updateSuperBlock(struct super_block *sb){
	FILE *sfs = fopen("sfs.bin", "r+");	
	fwrite(&sb, sizeof(sb), 1, sfs);
	fclose(sfs);
}



//TO FIND FIRST FREE INODENO 

int getFreeInodeNumber(struct super_block sb){
	int i;
	for(i = 0; i<32; i++){
		//AND OPERATION WITH 1 THEN CHECK IF ITS 0
		if((sb.inode_bitmap & 1) == 0){
			return i;
		}	
		else {
		//SHIFT TO RIGHT UNTIL FIND FIRST ZERO IF ITS 0 THEN IF BLOCK EXECUTES
			sb.inode_bitmap = sb.inode_bitmap >> 1;
		}
	}
	
}
//TO FIND FIRST FREE DATABLOCKNO
int getFreeDataBlockNumber(struct super_block sb){
	int i;
	int j;
	for(i=0; i<10;i++){ //INDEX 0-9
		for (j=0; j<32; j++ ) { //DataBlock 0-31
		if((sb.data_bitmap[i] & 1) == 0){		
		return (i*32) +j ;
	}
	else {
		sb.data_bitmap[i] = sb.data_bitmap[i] >> 1;
			}
		}	
	}
}

void mkdir() {
	int i;
	char dirName[32];
	scanf("%s", dirName);
// 4. Create an inode structure for the new directory 
	struct inode_st newDirectory;
	newDirectory.type = DIRECTORY;
	newDirectory.size = 64;
	
	FILE *sfs = fopen("sfs.bin", "r+");
// 1. Get a free inode number 	
	int  newInode = getFreeInodeNumber(sb);
	printf("new dir inodeno: %d\n",newInode);
// 3. Create two entries “dot” (.) and “dotdot”(..)	
	struct dir_ent dot;
	strcpy(dot.name,".");
	dot.inode_no = newInode;
	struct dir_ent dotdot;
	strcpy(dotdot.name,"..");
	dotdot.inode_no = curDirInodeNum;
	

// 2. Get a free datablock number 
	int datablock = getFreeDataBlockNumber(sb);
	
	printf("DataBlock: %d\n",datablock);
	
	for(i=0; i<10; i++){
		newDirectory.data_block_indices[i] = datablock; 	
		
	}

//0 to 9
	int	dataBlockIndex = datablock/32;
//0 to 31
	int position = datablock % 32;
	
//	printf("data blockindex: %d\n",dataBlockIndex);

// 5. Modify the super block (inode and data bitmap) 	
	updateINodeNo(&sb, newInode, 1);
	updateDataBlockNo(&sb, position, 1, dataBlockIndex);
	updateSuperBlock(&sb);
	
//Update the current directory size 		
	struct inode_st curinode = getCurInode();
	curinode.size += sizeof(struct dir_ent);
//write newinode to superblock	
	fseek(sfs, sizeof(struct super_block)+(curDirInodeNum*sizeof(struct inode_st)), SEEK_SET);
	fwrite(&curinode, sizeof(struct inode_st), 1, sfs);
//write the newdirectory	
//6. Write the inode structure to its location in I-Table 
	fseek(sfs, sizeof(struct super_block)+newInode*sizeof(struct inode_st), SEEK_SET);	
	fwrite(&newDirectory, sizeof(struct inode_st), 1, sfs);
//Jump to datablock	to new entries 
	fseek(sfs, sizeof(struct super_block)+(NUMOFINODES*sizeof(struct inode_st))+(newDirectory.data_block_indices[0]*DATABLOCKSIZE),SEEK_SET);
//7. Write the entries (. and ..) into the data block
	fwrite(&dot, sizeof(dot), 1, sfs);
	fwrite(&dotdot, sizeof(dotdot), 1, sfs);
	
//8. Add this directory as an entry in the current directory	
//We created new direntry to write new directory as an entry
	struct dir_ent newDirEntry;
	strcpy(newDirEntry.name, dirName);
	newDirEntry.inode_no = newInode;

	fseek(sfs, sizeof(struct super_block)+(NUMOFINODES*sizeof(struct inode_st))+(curinode.data_block_indices[0]*DATABLOCKSIZE) + (curinode.size-DIRENTRYSIZE),SEEK_SET);
	printf("new dir size: %d\n",newDirectory.size);
	
	fwrite(&newDirEntry, sizeof(struct dir_ent), 1, sfs);
	
	printf("current directory inodeno: %d\n",curDirInodeNum);	
	
	fclose(sfs);
	


}
