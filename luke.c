// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
// ID: 905381434, 905324354

#include <stdio.h>
#include <math.h>

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
        for(int byte_num = 0; byte_num != block_size; byte_num++)
        {
            int read_buffer;
            int bit_mask = 1;
            wrap_pread(fs_image, &read_buffer, 1, (group[group_num].bg_block_bitmap * block_size) + byte_num);
            
            // Look at one bit at a time
            for(int bit_num = 0; bit_num != 8; bit_num++)
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
    // might be able to get rid of the super block pointer because it's not used
}

void parse_directories(struct ext2_super_block *super, struct ext2_group_desc *group)
{
    // Check each group
    for(int group_num = 0; group_num != num_groups; group_num++)
    {
        // Check bytes in inode bitmap
        for(int byte_num = 0; byte_num != block_size; byte_num++)
        {
            int read_buffer;
            int bit_mask = 1;
            wrap_pread(fs_image, &read_buffer, 1, (group[group_num].bg_block_bitmap * block_size) + byte_num);

            // Look at one bit at a time
            for(unsigned int bit_num = 0; bit_num != 8; bit_num++)
            {
                if((byte_num * 8) + bit_num <= super->s_inodes_per_group && (bit_mask & read_buffer) != 0)
                {
                    int i_mode;
                    int i_mode_location = (group[group_num].bg_inode_table * block_size) + ((byte_num * 8) + bit_num - 1) * super->s_inode_size;
                    wrap_pread(fs_image, &i_mode, 2, i_mode_location);

                    // Check if entry is a directory
                    if(i_mode & 0x4000)
                    {
                        int dir_i_node_number = (group_num * super->s_inodes_per_group) + (byte_num * 8) + bit_num;

                        // Can finally parse directory
                        for(int i = 0; i != 12; i++)
                        {
                            unsigned int offset_data;
                            wrap_pread(fs_image, &offset_data, 4, i_mode_location + (i * 4) + 40);

                            if(offset_data != 0)
                            {
                                unsigned int offset_curr_entry = offset_data * block_size;

                                while(offset_curr_entry < (offset_data * block_size) + block_size)
                                {
                                    int len_entry;
                                    wrap_pread(fs_image, &len_entry, 2, offset_curr_entry + 4);

                                    struct ext2_dir_entry entry;
                                    wrap_pread(fs_image, &entry, 8 + len_entry, offset_curr_entry);

                                    printf("DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", dir_i_node_number, offset_curr_entry, entry.inode, entry.rec_len, entry.name_len, entry.name);

                                    offset_curr_entry += entry.rec_len;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void parse_indirect_blocks(struct ext2_super_block *super, struct ext2_group_desc *group)
{
    // Garbage to get compiler to shut up
    int i = super->s_blocks_count;
    int j = group->bg_block_bitmap;
    i++;
    j++;
    return;
}
