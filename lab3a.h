// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
// ID: 905381434, 905324354

#ifndef lab3a_h
#define lab3a_h

extern int fs_image;
extern int block_size;
extern int num_groups;

void parse_super(struct ext2_super_block *super); // KYLE
void parse_groups(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void parse_blocks_free(struct ext2_super_block *super, struct ext2_group_desc *group); // LUKE
void parse_inodes_free(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void inode_dirents(struct ext2_inode *inode, int inode_num); // LUKE
void inode_indirect(struct ext2_inode *inode, int inode_num); // LUKE

//extra helper functions
//defined in lab3a.c
ssize_t wrap_pread(int fd, void* buf, size_t nbyte, off_t offset);
int bd_to_offest(int block_id);

#endif /* lab3a_h */
