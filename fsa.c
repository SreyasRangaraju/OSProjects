#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2fs.h>
#include <math.h>
void list(int fd, int first, int last)
{
	unsigned char byte;
	int all[last-first+1];
	int i = first, start, end, cFlag = 0;
	while(i <= last)
	{
		read(fd,&byte,1);
		int n;
		for(n = 0;n < 8 && i <= last;n++)
		{
			all[i] = (byte&((unsigned char)ldexp(1,n)))/((unsigned char)ldexp(1,n));
			i++;
		}
	}
	for(i = first;i <= last;i++)
	{
		if(all[i] == 0 && (i == 0 || all[i-1] == 1))
		{
			start = i;
		}
		else if(all[i] == 1 && i > 0 && all[i-1] == 0)
		{
			end = i - 1;
			if(cFlag)
			{
				printf(", ");
			}
			if(start == end)
			{
				printf("%d",start);
			}
			else
			{
				printf("%d-%d",start,end);
				cFlag = 1;
			}
		}
		if(i == last && all[i] == 0)
		{
			end = i;
			if(cFlag)
			{
				printf(", ");
			}
			if(start == end)
			{
				printf("%d",start);
			}
			else
			{
				printf("%d-%d",start,end);
				cFlag = 1;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int fd, numGroup, bs;
	struct ext2_super_block *sb = malloc(sizeof(struct ext2_super_block));
	fd = open(argv[1], O_RDONLY);
	lseek(fd, 1024, SEEK_SET); 
	read(fd, sb, sizeof(struct ext2_super_block));
	numGroup = (sb->s_blocks_count+sb->s_blocks_per_group-1)/sb->s_blocks_per_group;
	printf("--General File System Information--\n");
	bs = (unsigned int)ldexp(1024,sb->s_log_block_size);
	printf("Block size in bytes: %u\n",bs);
	printf("Total Number of Blocks: %u\n", sb->s_blocks_count);
	printf("Disk Size in Bytes: %u\n",bs*sb->s_blocks_count);
	printf("Maximum Number of Blocks Per Group: %u\n", sb->s_blocks_per_group);
	printf("Inode Size in Bytes: %u\n", sb->s_inode_size);
	printf("Number of Inodes Per Group: %u\n", sb->s_inodes_per_group);
	printf("Number of Inode Blocks Per Group: %u\n",(sb->s_inodes_per_group*sb->s_inode_size+bs-1)/bs);
	printf("Number of Groups: %u\n\n",numGroup);
	printf("--Individual Group Information--\n");
	struct ext2_group_desc gd;
	int fblock = 0, lblock = sb->s_blocks_count - 1;
	lseek(fd, bs, SEEK_SET);
	read(fd, &gd, sizeof(struct ext2_group_desc));
	printf("-Group 1-\n");
	printf("Block IDs: %u-%u\n",fblock,lblock);
	printf("Block Bitmap Block ID: %u\n",gd.bg_block_bitmap);
	printf("Inode Bitmap Block ID: %u\n",gd.bg_inode_bitmap);
	printf("Inode Table Block ID: %u\n",gd.bg_inode_table);
	printf("Number of Free Blocks: %u\n",gd.bg_free_blocks_count);
	printf("Number of Free Inodes: %u\n",gd.bg_free_inodes_count);
	printf("Number of Directories: %u\n",gd.bg_used_dirs_count);
	printf("Free Block IDs: ");
	lseek(fd,gd.bg_block_bitmap*bs, SEEK_SET);
	list(fd, fblock, lblock);
	printf("\nFree Inode IDs: ");
	lseek(fd,gd.bg_inode_bitmap*bs, SEEK_SET);
	list(fd,fblock + 1,lblock + 1);
	printf("\n");
	printf("\n--Root Directory Entries--\n");
	
	lseek(fd,gd.bg_inode_table*bs+sb->s_inode_size, SEEK_SET);
	struct ext2_inode in;
	read(fd, &in, sizeof(struct ext2_inode));
	lseek(fd,in.i_block[0]*bs,SEEK_SET);
	
	struct ext2_dir_entry_2 *de;
	unsigned char block[4096];
	read(fd, block, bs);
	de = (struct ext2_dir_entry_2 *)block;
	while(de->rec_len > 0)
	{
		printf("Inode number: %u\n",de->inode);
		printf("Entry length: %u\n",de->rec_len);
		printf("Name length: %u\n",de->name_len);
		printf("File type: %u\n",de->file_type);
		printf("Name: %s\n\n",de->name);
		de = (void*) de + de->rec_len;
	}
	
	free(sb);
	close(fd);
	return 0;
}