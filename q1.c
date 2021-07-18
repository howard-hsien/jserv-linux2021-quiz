#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define __DECLARE_ROTATE(bits, type)                   \
    static inline type rotl##bits(const type v, int c) \
    {                                                  \
        const int mask = (bits) - (1);                 \
        c &= mask;                                     \
                                                       \
        return (v << c) | /*LLL*/(v>>(bits-c));                      \
    }                                                  \
                                                       \
    static inline type rotr##bits(const type v, int c) \
    {                                                  \
        const int mask = (bits) - (1);                 \
        c &= mask;                                     \
                                                       \
        return (v >> c) | /*RRR*/(v << (bits-c));                      \
    }

#define DECLARE_ROTATE(bits) __DECLARE_ROTATE(bits, uint##bits##_t)

DECLARE_ROTATE(64);
DECLARE_ROTATE(8);

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}
int main(int argc, char** argv){
    if(argc <5){
        printf("usage: ./q1.out <type> <dec> <shift> <'l'|'r'>\n");
        return -1; //format error
    }
    int i,j;
    char ** str;
    int type = atoi(argv[1]);
    uint64_t dec_64;
    uint32_t dec_32;
    uint16_t dec_16;
    uint8_t dec_8;
    if(type == 64){
        dec_64 = strtoul(argv[2], str, 10);
        printf("dec_64=%lu\n",dec_64);
    }

    else if(type == 32)
        dec_32 = atoi(argv[2]);
    else if(type == 16)
        dec_16 = atoi(argv[2]);
    else if(type == 8){
        dec_8 = atoi(argv[2]) & 0xff;
        printf("dec_8=%d\n", dec_8);
    }
    else
        printf("type can only be 64/32/16/8\n");
    int shift = atoi(argv[3]);
    
    for( i=0; i<argc; i++){
        printf("[%d]:%s\n", i, argv[i]); 
    }
    if(type == 64){
        if(!strcmp(argv[4],"l")){
            uint64_t result = rotl64(dec_64,shift);
            printf("result = %lu\n", result);
            printBits(sizeof(result), &result);
        }
        else if(!strcmp(argv[4],"r")){
            uint64_t result = rotr64(dec_64,shift);
            printf("result = %lu\n", result);
            printBits(sizeof(result), &result);
        }
    }
    else if(type == 8){
        if(!strcmp(argv[4],"l")){
            uint8_t result = rotl8(dec_8,shift);
            printf("result = %d\n", result);
            printBits(sizeof(result), &result);
        }
        else if(!strcmp(argv[4],"r")){
            uint8_t result = rotr8(dec_8,shift);
            printf("result = %d\n", result);
            printBits(sizeof(result), &result);
        }
    }
        

    
    printf("end\n");
    return 0;


}
