// NAME: Luke Bender, Kyle Whitecross
// EMAIL: lrbender01@g.ucla.edu, 
// ID: 905381434, 

#ifndef lab3a_h
#define lab3a_h

void parse_super(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void parse_groups(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void parse_blocks_free(struct ext2_super_block *super, struct ext2_group_desc *group); // LUKE
void parse_inodes_free(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void parse_inodes(struct ext2_super_block *super, struct ext2_group_desc *group); // KYLE
void parse_directories(struct ext2_super_block *super, struct ext2_group_desc *group); // LUKE
void parse_indirect_blocks(struct ext2_super_block *super, struct ext2_group_desc *group); // LUKE

//extra helper functions
ssize_t wrap_pread(int fd, void* buf, size_t nbyte, off_t offset);

#endif /* lab3a_h */
