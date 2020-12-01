// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, 
// ID: 905381434, 

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "ext2_fs.h"
#include "lab3a.h"

// relevant globals
const char* filepath;
int fs_image;

// helper functions
void usage() {
    fprintf(stderr, "usage: lab3a ext2_img\n");
}


int main(int argc, const char * argv[]) {
    // parse arguments
    if (argc != 2) {
        fprintf(stderr, "illegal number of arguments\n");
        usage();
        exit(1);
    }
    
    filepath = argv[1];
    fs_image = open(filepath, O_RDONLY);
    if (fs_image < 0) {
        fprintf(stderr, "can't open file %s: %s\n", filepath, strerror(errno));
        usage();
        exit(1);
    }
    
    
    // initialize variables
    // parse superblock
    // parse group
    // parse free blocks
    // parse free i-nodes
    // parse inodes
    // parse directories
    // parse indirect block references
    // close open file
    // exit
}
