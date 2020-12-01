// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, 
// ID: 905381434, 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "ext2_fs.h"
#include "lab3a.h"

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
    return;
}

void parse_inodes(struct ext2_super_block *super, struct ext2_group_desc *group) {
    return;
}
