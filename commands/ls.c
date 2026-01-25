#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, this is my ls command implementation\n");
    for(size_t i = 0; i < argc; i++) printf("argv[%zu]: %s\n", i, argv[i]);
    return 0;
}
