# NAME: Luke Bender, Kyle Whitecross
# EMAIL: lrbender01@g.ucla.edu, kswhitecross@gmail.com
# ID: 905381434, 905324354

all: lab3a.c luke.c kyle.c lab3a.h ext2_fs.h
	@ gcc -Wall -Wextra -o lab3a lab3a.c luke.c kyle.c -lm -g

dist:
	@ tar -czvf lab3a-905381434.tar.gz lab3a.c lab3a.h luke.c kyle.c ext2_fs.h README Makefile

clean:
	@ rm -rf lab3a lab3a-905381434.tar.gz
