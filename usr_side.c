#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
void error_msg() {
	printf("Invalid prog usage. Possible variants:\n");	
	printf("./usr_side -m\n");	
	printf("./usr_side -vma <pid>\n");	
}
 
int main(int argc, char *argv[])
{
 
	if (argc < 2) {
		error_msg();
	} else {
 
		int ans = -1;
 
		if (strcmp(argv[1], "-m") == 0) {
			ans = syscall(436);
 
		} 
 
		if (strcmp(argv[1], "-vma") == 0) {
			if (argc == 3) {
 
				int pid = atoi(argv[2]);
				ans = syscall(437, pid);
 
			} else {
				error_msg();
			}
		}
 
		if (ans == 0) {
				printf("SUCCESS\n");
		} else {printf("ERROR\n");}
 
	}
	return 0;
}
