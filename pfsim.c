#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 0xffff 

/* Page table entry */
typedef struct{
    bool        valid;
    uint16_t    pfn;
}pte;

/* Reverse mapping entry */
typedef struct{
    bool        unavail;
    uint8_t     proc;
    uint16_t    vpn;
}rme;

int main(int argc, char *argv[]){
    FILE *fp;
    char *filename;
    char *line = NULL;
    size_t len = 0;
    int read;
    char pid[2];
    char hex[7];
    char TYPE[2];
    int PID;
    int VPN;
    int disk_access;
    //int faults;
    //int page_access;
    pte PageTable[MAX_SIZE];


    //check if file arguement is passed
    if (argc < 2){
        printf("Usage: ref_pfsim <file>\n");
        return(1);
    } else{
        filename = argv[1];
    }
    //open file
    fp = fopen(filename, "r");
    while (1){
        while((read = getline(&line, &len, fp)) != -1){
            //Read line into 3 variables
            strncpy(pid, line, 1);
            pid[2] = '\0';
            PID = (int)strtol(pid, NULL, 0);
            strncpy(hex, line + 2, 6);
            hex[7] = '\0';
            VPN = (int)strtol(hex, NULL, 0);
            strncpy(TYPE, line + 9, 1);
            TYPE[2] = '\0';
            if (PID == 0){
                printf("Found");
                PageTable[VPN];
            }
            //Create page table entry for VPN
            

            printf("%d", PID);
            printf("%d", VPN);
            //printf("%s\n", TYPE);
            //printf("%s\n", line);
            disk_access++;
        }
        break;
    }
    fclose(fp);
    //printf("\nPage accesses: %d", page_access);
    //printf("\nPage Faults: %d", faults);
    //printf("\nDisk accesses: %d", disk_access);
}
