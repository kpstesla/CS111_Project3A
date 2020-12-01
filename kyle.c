// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
// ID: 905381434, 905324354

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "ext2_fs.h"
#include "lab3a.h"

#define MAX_INODES 64
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

extern int fs_image;
extern int block_size;

void parse_super(struct ext2_super_block *super, struct ext2_group_desc *group) {
    //title
    printf("SUPERBLOCK,");
    
    //total number of blocks and inodes
    printf("%d,", super->s_blocks_count);
    printf("%d,", super->s_inodes_count);
    
    //block and inode size
    printf("%d,", block_size);
    printf("%d,", super->s_inode_size);
    
    //blocks and inodes per group
    printf("%d,", super->s_blocks_per_group);
    printf("%d,", super->s_inodes_per_group);
    
    //first non-reserved inode
    printf("%d\n", super->s_first_ino);
    return;
}

void parse_groups(struct ext2_super_block *super, struct ext2_group_desc *group) {
    //title
    printf("GROUP,");
    printf("%d,", 0);
    
    //total number of blocks and inodes in this group
    int group_blocks = super->s_blocks_count;
    int group_inodes = super->s_inodes_count;
    printf("%d,", group_blocks);
    printf("%d,", group_inodes);
    
    //free blocks and inodes
    printf("%d,", group->bg_free_blocks_count);
    printf("%d,", group->bg_free_inodes_count);
    
    //block ids for block and inode bitmaps
    printf("%d,", group->bg_block_bitmap);
    printf("%d,", group->bg_inode_bitmap);
    
    //block id for inode table
    printf("%d\n", group->bg_inode_table);
    return;
}

void parse_inodes_free(struct ext2_super_block *super, struct ext2_group_desc *group) {
    //get the bitmap
    int bitmap_id = group->bg_inode_bitmap;
    unsigned char* i_bitmap = (unsigned char*) malloc(block_size);
    wrap_pread(fs_image, i_bitmap, block_size, bd_to_offest(bitmap_id));
    
    //get the start of the inode table
    int itable_offset = bd_to_offest(group->bg_inode_table);
    int inode_size = super->s_inode_size;
    struct ext2_inode current_inode;
    
    
    //get the number of inodes
    int num_inodes = super->s_inodes_count;
    int inode_number;
    unsigned char temp;
    for (int i = 0; i < ceil(num_inodes / 8); i++) {
        temp = i_bitmap[i];
        
        //check if we don't want to process the whole next byte
        int last_bit = min(num_inodes - i * 8, 8);
        for (int j = 0; j < last_bit; j++) {
            //Inodes are 1-indexed
            inode_number = i * 8 + j + 1;
            
            //if inode is free
            if (!(temp & 0x1)) {
                printf("IFREE,%d\n", inode_number);
            }
            
            //else inode is allocated
            else {
                //read in the inode
                wrap_pread(fs_image, &current_inode, inode_size, itable_offset + (inode_number - 1) * inode_size);
                
                //only process inodes with a non-zero mode and non-zero link count
                if ((current_inode.i_mode & 0xf000) != 0) {
                    if (current_inode.i_links_count != 0) {
                        //title
                        printf("INODE,");
                        
                        //number
                        printf("%d,", inode_number);
                        
                        //file type
                        char type;
                        switch (current_inode.i_mode & 0xf000) {
                            case 0xa000:
                                type = 's';
                                break;
                            case 0x8000:
                                type = 'f';
                                break;
                            case 0x4000:
                                type = 'd';
                                break;
                            default:
                                type = '?';
                                break;
                        }
                        printf("%c,", type);
                        
                        //mode
                        printf("%03o,", current_inode.i_mode & 0x0fff);
                        
                        //owner uid
                        printf("%hd,", current_inode.i_uid);
                        
                        //group
                        printf("%hd,", current_inode.i_gid);
                        
                        //link count
                        printf("%hd,", current_inode.i_links_count);
                        
                        //last modification time
                        struct tm* mt = gmtime(&current_inode.i_mtime);
                        printf("%02d/%02d/%02d %02d:%02d:%02d,",
                               mt->tm_mon + 1, mt->tm_mday, mt->tm_year - 100,
                               mt->tm_hour, mt->tm_min, mt->tm_sec);
                        
                        
                        //newline
                        printf("\n");
                    }
                }
            }
            temp = temp >> 1;
        }
    }
    free(i_bitmap);
    return;
}

void parse_inodes(struct ext2_super_block *super, struct ext2_group_desc *group) {
    return;
}
