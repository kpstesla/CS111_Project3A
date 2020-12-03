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

void parse_entry_block(unsigned int block_num, int inode_num)
{
    struct ext2_dir_entry entry;
    unsigned int entry_offset = block_num * block_size;
    unsigned int start_offset = block_num * block_size;

    // Look through entire block
    while(entry_offset < (block_num * block_size) + block_size)
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
            // Parse entries from block
            parse_entry_block(block_number, inode_num);
        }
    }
    return;
}

void inode_indirect(struct ext2_inode *inode, int inode_num)
{
    // Singly indirect
    if(inode->i_block[12] != 0)
    {
        unsigned int num_block_ptrs = block_size / sizeof(u_int32_t);
        unsigned int *blocks = (unsigned int*)malloc(block_size);
        wrap_pread(fs_image, blocks, block_size, inode->i_block[12] * block_size);

        // Look through each block pointed to
        for(unsigned int block_num = 0; block_num != num_block_ptrs; block_num++)
        {
            // Check that block isn't empty
            if(blocks[block_num] != 0)
            {
                // Print
                printf("INDIRECT,%d,%d,%d,%d,%d\n",
                    inode_num, // inode number
                    1, // level of indirection
                    block_num + 12, // block offset
                    inode->i_block[12], // block number of indirect block
                    blocks[block_num] // block number of referenced block
                );

                // If this is an indirect directory entry
                if((inode->i_mode & 0xf000) == 0x4000)
                    parse_entry_block(blocks[block_num], inode_num);
            }
        }
        free(blocks);
    }

    // Doubly indirect
    if(inode->i_block[13] != 0)
    {
        unsigned int num_block_ptrs = block_size / sizeof(u_int32_t);
        unsigned int *block_ptrs = (unsigned int*)malloc(block_size);
        wrap_pread(fs_image, block_ptrs, block_size, inode->i_block[13] * block_size);

        // Look at each block pointer being pointed to
        for(unsigned int block_ptr_num = 0; block_ptr_num != num_block_ptrs; block_ptr_num++)
        {
            // Check that pointer isn't empty
            if(block_ptrs[block_ptr_num] != 0)
            {
                // Print
                printf("INDIRECT,%d,%d,%d,%d,%d\n",
                    inode_num, // inode number
                    2, // level of indirection
                    block_ptr_num * 12, // block offset // NOT SURE AT ALL ON THIS ONE, TODO
                    inode->i_block[13], // block number of indirect block
                    block_ptrs[block_ptr_num] // block number
                );

                unsigned int *blocks = (unsigned int*)malloc(block_size);
                wrap_pread(fs_image, blocks, block_size, block_ptrs[block_ptr_num] * block_size);

                // Look through each block being pointed to
                for(unsigned int block_num = 0; block_num != num_block_ptrs; block_num++)
                {
                    // Check that block isn't empty
                    if(blocks[block_num] != 0)
                    {
                        // Print
                        printf("INDIRECT,%d,%d,%d,%d,%d\n",
                            inode_num, // inode number
                            1, // level of indirection
                            block_ptr_num * 12 + block_num, // block offset // NOT SURE AT ALL ON THIS ONE EITHER, TODO
                            block_ptrs[block_ptr_num], // block number of indirect block
                            blocks[block_num] // block number of referenced block
                        );

                        // If this is an indirect directory entry
                        if((inode->i_mode & 0xf000) == 0x4000)
                            parse_entry_block(blocks[block_num], inode_num);                        
                    }
                }
                free(blocks);        
            }
        }
        free(block_ptrs);
    }

    // Triply indirect
    if(inode->i_block[14] != 0)
    {
        unsigned int num_block_ptrs = block_size / sizeof(u_int32_t);
        unsigned int *block_ptr_ptrs = (unsigned int*)malloc(block_size);
        wrap_pread(fs_image, block_ptr_ptrs, block_size, inode->i_block[14] * block_size);

        // Look at each block pointer pointer being pointed to
        for(unsigned int block_ptr_ptr_num = 0; block_ptr_ptr_num != num_block_ptrs; block_ptr_ptr_num++)
        {
            // Check that block pointer pointer isn't empty
            if(block_ptr_ptrs[block_ptr_ptr_num] != 0)
            {
                // Print
                printf("INDIRECT,%d,%d,%d,%d,%d\n",
                    inode_num, // inode number
                    3, // level of indirection
                    block_ptr_ptr_num * 144, // block offset // NOT SURE AT ALL ON THIS ONE, TODO
                    inode->i_block[14], // block number of indirect block
                    block_ptr_ptrs[block_ptr_ptr_num] // block number
                );

                unsigned int *block_ptrs = (unsigned int*)malloc(block_size);
                wrap_pread(fs_image, block_ptrs, block_size, block_ptr_ptrs[block_ptr_ptr_num] * block_size);

                // Look through each block pointer being pointed to
                for(unsigned int block_ptr_num = 0; block_ptr_num != num_block_ptrs; block_ptr_num++)
                {
                    // Check that block pointer isn't empty
                    if(block_ptrs[block_ptr_num] != 0)
                    {
                        // Print
                        printf("INDIRECT,%d,%d,%d,%d,%d\n",
                            inode_num, // inode number
                            2, // level of indirection
                            block_ptr_ptr_num * 144 + block_ptr_num * 12, // block offset // NOT SURE AT ALL ON THIS ONE, TODO
                            block_ptr_ptrs[block_ptr_ptr_num], // block number of indirect block
                            block_ptrs[block_ptr_num] // block number
                        );

                        unsigned int *blocks = (unsigned int*)malloc(block_size);
                        wrap_pread(fs_image, blocks, block_size, block_ptrs[block_ptr_num]);

                        // Look through each block being pointed to
                        for(unsigned int block_num = 0; block_num != num_block_ptrs; block_num++)
                        {
                            // Check that block isn't empty
                            if(blocks[block_num] != 0)
                            {
                                // Print
                                printf("INDIRECT,%d,%d,%d,%d,%d\n",
                                    inode_num, // inode number
                                    1, // level of indirection
                                    block_ptr_ptr_num * 144 + block_ptr_num * 12 + block_num, // block offset // NOT SURE AT ALL ON THIS ONE EITHER, TODO
                                    block_ptrs[block_ptr_num], // block number of indirect block
                                    blocks[block_num] // block number of referenced block
                                );

                                // If this is an indirect directory entry
                                if((inode->i_mode & 0xf000) == 0x4000)
                                    parse_entry_block(blocks[block_num], inode_num);  
                            }
                        }
                        free(blocks);
                    }
                }
                free(block_ptrs);
            }
        }
        free(block_ptr_ptrs);
    }

    return;
}
