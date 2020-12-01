// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, 
// ID: 905381434, 

#include <stdio.h>
#include <math.h>
#include <sys/types.h>

#include "ext2_fs.h"
#include "lab3a.h"

extern int fs_image;

void parse_blocks_free(struct ext2_super_block *super, struct ext2_group_desc *group)
{
    int num_groups = ceil((double)(super->s_blocks_count) / super->s_blocks_per_group);

    // Go through each group
    for(int i = 0; i != num_groups; i++)
    {
        // Read and analyze one byte at a time
        for(u_int32_t j = 0; j != super->s_log_block_size; j++)
        {
            u_int8_t buffer_8;
            u_int8_t bit_mask = 1;
            wrap_pread(fs_image, &buffer_8, 1, (group[i].bg_block_bitmap * super->s_log_block_size) + j);
            bit_mask++;
        }
    }
    return;
}

void parse_directories(struct ext2_super_block *super, struct ext2_group_desc *group) {
    // Garbage to get compiler to shut up
    int i = super->s_blocks_count;
    int j = group->bg_block_bitmap;
    i++;
    j++;
    return;
}

void parse_indirect_blocks(struct ext2_super_block *super, struct ext2_group_desc *group) {
    // Garbage to get compiler to shut up
    int i = super->s_blocks_count;
    int j = group->bg_block_bitmap;
    i++;
    j++;
    return;
}