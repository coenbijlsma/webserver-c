#include "log/cws_log.h"
#include "net/inet.h"
#include "datastructures/hashmap.h"
#include <stdio.h>

int main(int argc, char** argv) {
    printf("__wordsize == %d\n", __WORDSIZE);
    cws_log(FATAL, "fatal message");
    cws_log(ERROR, "error message");
    cws_log(WARNING, "warning message");
    cws_log(INFO, "info message");
    cws_log(DEBUG, "debug message");
    
    // %llx = x86, %lx = x64
    printf("%llx\n", 0xf1f2f3f4f5f6f7f8);
    printf("%llx\n", htonll(0xf1f2f3f4f5f6f7f8));
    char line[50];
   
    printf("enter a string: ");
    gets(line);
    printf("hash of [%s] is [%u]\n", line, fnv1a_hash(line));
    return 0;
}
