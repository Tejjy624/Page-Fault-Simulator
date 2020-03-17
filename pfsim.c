#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int main(int argc, char *argv[]){
    FILE *fp;
    char *filename;
    char ch;

    if (argc < 2){
        printf("Usage: ref_pfsim <file>");
        return 1;
    } else{
        filename = argv[1];
        printf("Filename: %s\n", filename);
    }
    fp = fopen(filename, "r");
    if (fp){
        printf("File contents:\n");
        while ((ch = fgetc(fp)) != EOF){
            printf("%c", ch);
        }
    } else{
        printf("Failed to open the file\n");
    }
    return 0;
}
    