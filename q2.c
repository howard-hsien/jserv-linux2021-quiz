#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static inline uintptr_t align_up(uintptr_t sz, size_t alignment)
{
    uintptr_t mask = alignment - 1;
    if ((alignment & mask) == 0) {  /* power of two? */
        return ((sz+mask)  & ~mask);//MMM       
    }
    return (((sz + mask) / alignment) * alignment);
}

int main(int argc, char ** argv){
	if(argc < 3){
		perror("./q2.out <sz> <alignment>\n");
		return -1;
	}
	int sz = atoi(argv[1]);
	int alignment = atoi(argv[2]);
	printf("allign_up(%d,%d)=%ld\n", sz, alignment, align_up(sz, alignment));
	return 0;

}
