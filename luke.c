// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
// ID: 905381434, 905324354

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "ext2_fs.h"
#include "lab3a.h"

extern int fs_image;
extern int block_size;
extern int num_groups;

void parse_blocks_free(struct ext2_super_block *super, struct ext2_group_desc *group)
{
    // Go through each group
    for(int group_num = 0; group_num != num_groups; group_num++)
    {
        // Read and analyze one byte at a time
        for(int byte_num = 0; byte_num != ceil(super->s_blocks_count / 8); byte_num++)
        {
            int read_buffer;
            int bit_mask = 1;
            wrap_pread(fs_image, &read_buffer, 1, (group[group_num].bg_block_bitmap * block_size) + byte_num);
            
            int last_valid_bit = fmin(super->s_blocks_count - byte_num * 8, 8);

            // Look at one bit at a time
            for(int bit_num = 0; bit_num != last_valid_bit; bit_num++)
            {
                // Check if the block is free
                if((bit_mask & read_buffer) == 0)
                    printf("BFREE,%d\n", (byte_num * 8) + bit_num + 1);
                
                // Update bit_mask
                bit_mask = bit_mask << 1;
            }
        }
    }
    return;
}

void inode_dirents(struct ext2_inode *inode, int inode_num)
{
    // Iterate through direct data blocks in directory inode
    for(unsigned int inode_block_num = 0; inode_block_num != 12; inode_block_num++)
    {
        // Get the block number
        unsigned int block_number;
        block_number = inode->i_block[inode_block_num];

        // Only look in valid blocks
        if(block_number != 0)
        {
            struct ext2_dir_entry entry;
            unsigned int entry_offset = block_number * block_size;
            unsigned int start_offset = block_number * block_size;

            // Look through entire block
            while(entry_offset < (block_number * block_size) + block_size)
            {
                // Read in the entire entry irrespective of name length
                wrap_pread(fs_image, &entry, sizeof(struct ext2_dir_entry), entry_offset);

                // Only print valid entries found
                if(entry.inode != 0)
                    printf("DIRENT,%d,%d,%d,%d,%d,\'%s\'\n",
                        inode_num, // directory inode number
                        entry_offset - start_offset, // logical byte offset in block
                        entry.inode, // entry inode number
                        entry.rec_len, // entry length
                        entry.name_len, // entry name length
                        entry.name // entry name
                    );

                // Update offset for next entry
                entry_offset += entry.rec_len;
            }
        }
    }
    return;
}

void inode_indirect(struct ext2_super_block *super, struct ext2_group_desc *group, struct ext2_inode *inode, int inode_num) {
    // Garbage to get compiler to shut up
    int i = super->s_blocks_count;
    int j = group->bg_block_bitmap;
    int k = inode->i_blocks + inode_num;
    i++;
    j++;
    k++;

    // Singly indirect
    if(inode->i_block[12] != 0)
    {
        unsigned int num_block_ptrs = block_size / 4;
        unsigned int *block = (unsigned int*)malloc(block_size);
        wrap_pread(fs_image, block, block_size, inode->i_block[12] * block_size);

        for(unsigned int block_num = 0; block_num != num_block_ptrs; block_num++)
        {
            if(block[block_num] != 0)
            {
                printf("INDIRECT,%d,%d,%d,%d,%d\n",
                    inode_num,
                    1,
                    block_num + 12,
                    inode->i_block[12],
                    *block
                );
            }
        }
        free(block);
    }

    // Doubly indirect
    if(inode->i_block[13] != 0)
    {
        
    }

    // Triply indirect
    if(inode->i_block[14] != 0)
    {
        
    }

    return;
}
