// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
// ID: 905381434, 905324354

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "ext2_fs.h"
#include "lab3a.h"

#define MAX_INODES 64

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
    
    //get the number of inodes
    int num_inodes = super->s_inodes_count;
    unsigned char temp;
    for (int i = 0; i < num_inodes; i++) {
        temp = i_bitmap[i];
        for (int j = 0; j < 8; j++) {
            if (!(temp & 0x1)) {
                //Inodes are 1-indexed
                printf("IFREE,%d\n", i * 8 + j + 1);
            }
            temp = temp >> 1;
        }
    }
    free(i_bitmap);
    return;
}

void parse_inodes(struct ext2_super_block *super, struct ext2_group_desc *group) {
    //get the inode bitmap
    int bitmap_id = group->bg_inode_bitmap;
    unsigned char* i_bitmap = (unsigned char*) malloc(block_size);
    wrap_pread(fs_image, i_bitmap, block_size, bd_to_offest(bitmap_id));
    
    //get the start of the inode table
    int inode_table_offset = bd_to_offest(group->bg_inode_table);
    
    //inode info
    int num_inodes = super->s_inodes_count;
    int inode_size = super->s_inode_size;
    int inodes_processed = 0;
    int inodes_to_process;
    struct ext2_inode inodes[MAX_INODES];
    
    //process inodes until done
    while (inodes_processed != num_inodes) {
        //read in the inodes
        if (num_inodes - inodes_processed > MAX_INODES) {
            inodes_to_process = MAX_INODES;
        }
        else {
            inodes_to_process = num_inodes - inodes_processed;
        }
        wrap_pread(fs_image, inodes, inodes_to_process * sizeof(struct ext2_inode), inode_table_offset + inodes_processed * sizeof(struct ext2_inode));
        inodes_processed += inodes_to_process;
        
        //iterate over the bitmap and process analyzed inodes
    }
    free(i_bitmap);
}
