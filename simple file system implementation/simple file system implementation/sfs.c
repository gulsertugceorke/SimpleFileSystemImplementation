#include"sfsHeader.h"
#include <stdio.h>
#include <string.h>

int main(){
	int i = 0;
	struct super_block sb;
	initialize_sb(sb);
	
	struct dir_ent entry;
	struct inode_st root; 
	//inode için iþlemler
	root.type = DIRECTORY; 
//empty excepts 2 things - . and .. every file has that
	root.size = sizeof(struct dir_ent)*2;
//0. DATABLOCKU ROOTA VERÝYORUZ	
	for (i = 0; i< 10; i++){
		root.data_block_indices[i] = 0;
	}
	//bitti
	
//superblock için iþlemler
	
//first inode structure is being used now	
//first datablock is being used now
	sb.inode_bitmap = 1;
	sb.data_bitmap[0] = 1;	
	//bitti 
	printf("size of directory: %lu\n",sizeof(struct dir_ent));
	
	struct dir_ent dot;
	strcpy(dot.name,".");
	dot.inode_no = curDirInodeNum;
	struct dir_ent dotdot;
	strcpy(dotdot.name,"..");
	dotdot.inode_no = curDirInodeNum; 
	
	FILE *sfs = fopen("sfs.bin", "w+");
	fwrite(&sb, sizeof(struct super_block),1,sfs);
	
	//write root
	fwrite(&root, sizeof(struct inode_st),1,sfs);
	//jump to datablock
	fseek(sfs, sizeof(sb) +NUMOFINODES*sizeof(struct inode_st)+(root.data_block_indices[0]*DATABLOCKSIZE), SEEK_SET);
	//write dots
	fwrite(&dot, sizeof(struct dir_ent), 1, sfs);
	fwrite(&dotdot, sizeof(struct dir_ent), 1, sfs);
	//close file	
	fclose(sfs); 

	}
