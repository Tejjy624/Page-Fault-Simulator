#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 128
#define BLOCK_SIZE 512 

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
    bool        referenced;
    bool        dirty;
}rme;

pte PageTable[4][MAX_SIZE];
rme FrameTable[32];
int mem_access=0;
int count = 0;
int faults=0;
int disk_access=0;
int PID, VPN, PFN;
int OldPID, OldVPN;
char pid[2];
char hex[7];
char TYPE[2];
int temp;

int getvpn(int n){
    int a[16], i;
    int rem, bin, dec = 0, base = 1;
    char str[10];
    int num;
    for (i =0; n>0; i++){
        a[i] = n%2;
        n=n/2;
    }
    for (i = i-1; i>=9; i--){
        num = a[i];
        sprintf(str,"%s%d", str, num);
    }
    bin = atoi(str);
    while(bin){
        rem = bin % 10;
        dec = dec + rem * base;
        bin = bin/10;
        base = base * 2;
    }
    return dec;
}

void evict(){
    //Unreferenced page with dirty off
    for (int i = 0; i<32; i++){
        if (FrameTable[i].referenced == 0){
            if (FrameTable[i].dirty == 0){
                //Modify original PTE
                OldPID = FrameTable[i].proc;
                OldVPN = FrameTable[i].vpn;
                PageTable[OldPID][OldVPN].valid = 0;

                //Modify FrameTable with new values
                FrameTable[i].proc = PID;
                FrameTable[i].vpn = VPN;
                FrameTable[i].unavail = 1;
                FrameTable[i].referenced = 1;
                PageTable[PID][VPN].valid = 1;
                PageTable[PID][VPN].pfn = i;
                return;
            }
        }
    }
    //unreferenced page with dirty on
    for (int i = 0; i<32; i++){
        if (FrameTable[i].referenced == 0){
            if (FrameTable[i].dirty == 1){
                //Modify original PTE
                OldPID = FrameTable[i].proc;
                OldVPN = FrameTable[i].vpn;
                PageTable[OldPID][OldVPN].valid = 0;

                //Modify FrameTable with new values
                FrameTable[i].proc = PID;
                FrameTable[i].vpn = VPN;
                FrameTable[i].unavail = 1;
                FrameTable[i].referenced = 1;
                PageTable[PID][VPN].valid = 1;
                PageTable[PID][VPN].pfn = i;
                disk_access++;
                return;
            }
        }
    }
    //referenced page with dirty off
    for (int i = 0; i<32; i++){
        if (FrameTable[i].referenced == 1 && FrameTable[i].dirty == 0){
            //Modify original PTE
            OldPID = FrameTable[i].proc;
            OldVPN = FrameTable[i].vpn;
            PageTable[OldPID][OldVPN].valid = 0;

            //Modify FrameTable with new values
            FrameTable[i].proc = PID;
            FrameTable[i].vpn = VPN;
            FrameTable[i].unavail = 1;
            FrameTable[i].referenced = 1;
            PageTable[PID][VPN].valid = 1;
            PageTable[PID][VPN].pfn = i;
            return;
        }
    }
    //referenced page with dirty on
    for (int i = 0; i<32; i++){
        if (FrameTable[i].referenced == 1 && FrameTable[i].dirty == 1){
            //Modify original PTE
            OldPID = FrameTable[i].proc;
            OldVPN = FrameTable[i].vpn;
            PageTable[OldPID][OldVPN].valid = 0;

            //Modify FrameTable with new values
            FrameTable[i].proc = PID;
            FrameTable[i].vpn = VPN;
            FrameTable[i].unavail = 1;
            FrameTable[i].referenced = 1;
            PageTable[PID][VPN].valid = 1;
            PageTable[PID][VPN].pfn = i;
            disk_access++;
            return;
        }
    }
}

int main(int argc, char *argv[]){
    FILE *fp;
    char *filename;
    char *line = NULL;
    size_t len = 0;
    int read;
    //check if file arguement is passed
    if (argc < 2){
        printf("Usage: ref_pfsim <file>\n");
        return(1);
    } else{
        filename = argv[1];
    }
    //open file
    fp = fopen(filename, "r");

    while((read = getline(&line, &len, fp)) != -1){
        //Read line into 3 variables
        strncpy(pid, line, 1);
        pid[2] = '\0';
        PID = (int)strtol(pid, NULL, 0);
        strncpy(hex, line + 2, 6);
        hex[7] = '\0';
        temp = strtol(hex, NULL, 0);
        VPN = getvpn(temp);
        strncpy(TYPE, line + 9, 1);
        TYPE[1] = '\0';

        if (count == 200){
            for (int i = 0; i<32; i++){
                if (FrameTable[i].referenced == 1){
                    FrameTable[i].referenced = 0;
                }
            }
            count = 0;
        }

        //Locate page table entry for VPN
        if (PageTable[PID][VPN].valid != 1){
            //Page Fault
            faults++;
            //Find a free frame
            for (int i = 0; i <32; i++){
                if (FrameTable[i].unavail == 0){
                    disk_access++;
                    FrameTable[i].unavail = 1;
                    FrameTable[i].proc = PID;
                    FrameTable[i].vpn = VPN;
                    FrameTable[i].referenced = 1;
                    PageTable[PID][VPN].valid = 1;
                    PageTable[PID][VPN].pfn = i;
                    if (strcmp(TYPE,"W") == 0){
                        FrameTable[i].dirty = 1;
                    }
                    break;
                }else{
                    disk_access++;
                    evict();
                    break;
                }
            }
        }
        if (PageTable[PID][VPN].valid == 1){
            PFN = PageTable[PID][VPN].pfn;
            FrameTable[PFN].referenced = 1;
            if (strcmp(TYPE,"W") == 0){
                FrameTable[PFN].dirty = 1;
            }
        }
        count++;
        mem_access++;
    }
    fclose(fp);
    printf("Page accesses: %d\n", mem_access);
    printf("Page Faults: %d\n", faults);
    printf("Disk accesses: %d\n", disk_access);
}
