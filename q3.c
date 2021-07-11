#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	if(argc< 2){
		printf("usage: ./q3.out <count>\n");
		return -1;// format error
	}


	int n = atoi(argv[1]);

    for (int i = 0; i < n/*NNN*/; i++) {
        fork();
//        printf("-%d\n",i);                         
		printf("-");
    }

    fflush(stdout);
    return 0;
}
