// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, 
// ID: 905381434, 

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "ext2_fs.h"
#include "lab3a.h"

//relevant constants
#define SUPERBLOCK_OFFSET 1024

// relevant globals
const char* filepath;
int fs_image;
int block_size;
int num_groups;

// helper functions
void usage() {
    fprintf(stderr, "Usage: lab3a EXT2_IMAGE\n");
}

//calls pread, but exits if rc < 0
ssize_t wrap_pread(int fd, void* buf, size_t nbyte, off_t offset) {
    ssize_t rc = pread(fd, buf, nbyte, offset);
    if (rc < 0) {
        fprintf(stderr, "Error reading %d: %s\n", errno, strerror(errno));
        exit(1);
    }
    return rc;
}

//returns offset from block_id
//0 -> boot
//1 -> super
//2 -> start of group descriptor array
int bd_to_offest(int block_id) {
    return block_size * block_id;
}


int main(int argc, const char * argv[]) {
    // parse arguments
    if (argc != 2) {
        fprintf(stderr, "Illegal number of arguments\n");
        usage();
        exit(1);
    }
    
    filepath = argv[1];
    fs_image = open(filepath, O_RDONLY);
    if (fs_image < 0) {
        fprintf(stderr, "Error opening file %s: %s\n", filepath, strerror(errno));
        usage();
        exit(1);
    }
    
    // initialize variables
    struct ext2_super_block super_block;
    wrap_pread(fs_image, &super_block, sizeof(struct ext2_super_block), SUPERBLOCK_OFFSET);
    
    if (super_block.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "super_block magic number does not match\n");
        exit(1);
    }
    block_size = EXT2_MIN_BLOCK_SIZE << super_block.s_log_block_size;
    
    num_groups = ceil( (double) super_block.s_blocks_count / super_block.s_blocks_per_group);
    if (num_groups != ceil( (double) super_block.s_inodes_count / super_block.s_inodes_per_group)) {
        //error noting
        fprintf(stderr, "Inconsistent number of groups\n");
        fprintf(stderr, "Inodes: %d, per group: %d, groups: %f\n",
                super_block.s_inodes_count,
                super_block.s_inodes_per_group,
                ceil((double) super_block.s_inodes_count / super_block.s_inodes_per_group));
        fprintf(stderr, "Blocks: %d, per group: %d, groups: %d\n",
                super_block.s_blocks_count,
                super_block.s_blocks_per_group,
                num_groups);
        exit(1);
    }
    
    struct ext2_group_desc* group_descriptors = malloc(sizeof(struct ext2_group_desc) * num_groups);
    wrap_pread(fs_image, group_descriptors, sizeof(struct ext2_group_desc) * num_groups, SUPERBLOCK_OFFSET + block_size);
    
    // parse superblock
    parse_super(&super_block, &group_descriptors[0]);
    
    // parse group
    parse_groups(&super_block, &group_descriptors[0]);
    
    // parse free blocks
    parse_blocks_free(&super_block, &group_descriptors[0]);
    
    // parse free i-nodes
    parse_inodes_free(&super_block, &group_descriptors[0]);
    
    // parse inodes
    parse_inodes(&super_block, &group_descriptors[0]);
    
    // parse directories
    parse_directories(&super_block, &group_descriptors[0]);
    
    // parse indirect block references
    parse_indirect_blocks(&super_block, &group_descriptors[0]);
    
    // close open resources
    close(fs_image);
    free(group_descriptors);
    
    // exit
    exit(0);
}
