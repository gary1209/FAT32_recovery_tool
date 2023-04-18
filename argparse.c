#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>

#include "argparse.h"


void open_disk(char *disk_name, void **data) {
    int fd = open(disk_name, O_RDWR);
    if (fd == -1) {
        perror("Error: Cannot open disk\n");
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error: Cannot get disk size\n");
        close(fd);
    }
    *data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}


void print_fs_info(void *data) {
    BootEntry *ptr = (BootEntry *)data;
    printf("Number of FATs = %d\n", ptr->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", ptr->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", ptr->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", ptr->BPB_RsvdSecCnt);
}


// void list_root_dir(void *data) {
//     BootEntry *ptr = (BootEntry *)data;
//     unsigned int clus = ptr->BPB_RootClus;
//     while (clus < 0x0ffffff8) {

//     }
// }


void print_usage() {
    printf("Usage: ./nyufile disk <options>\n");
    printf("  -i                     Print the file system information.\n");
    printf("  -l                     List the root directory.\n");
    printf("  -r filename [-s sha1]  Recover a contiguous file.\n");
    printf("  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
}


void validate_flags(int flag_i, int flag_l, int flag_r, int flag_R, int flag_s) {
    if ((!flag_i && !flag_l && !flag_r && !flag_R && !flag_s) || \
       (flag_i && (flag_l || flag_r || flag_R || flag_s)) || \
       (flag_l && (flag_r || flag_R || flag_s)) || \
       (flag_s && !flag_r && !flag_R) || \
       (!flag_s && flag_R)) {
        print_usage();
        exit(1);
    }
}


void parse_args(int argc, char *argv[], char *disk_name) {
    if (argc < 2) {
        print_usage();
        exit(1);
    }

    int opt;
    int flag_i = 0;
    int flag_l = 0;
    int flag_r = 0;
    int flag_R = 0;
    int flag_s = 0;
    void *data = NULL;

    while((opt = getopt(argc, argv, "ilr:R:s:")) != -1) 
    { 
        switch(opt) 
        { 
            case 'i':
                flag_i = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            case 'r':
                flag_r = 1;
                if (strstr(optarg, "-") != NULL) {
                    print_usage();
                    exit(1);
                }
                break;
            case 'R':
                flag_R = 1;
                if (strstr(optarg, "-") != NULL) {
                    print_usage();
                    exit(1);
                }
                break;
            case 's':
                flag_s = 1;
                if (strstr(optarg, "-") != NULL) {
                    print_usage();
                    exit(1);
                }
                break;
            
            default:
                print_usage();
                exit(1);
        } 
    } 
    if (argv[optind] == NULL) {
        print_usage();
        exit(1);
    }
    
    strcpy(disk_name, argv[optind]);
    
    validate_flags(flag_i, flag_l, flag_r, flag_R, flag_s);

    if (flag_i) {
        open_disk(disk_name, &data);
        print_fs_info(data);
    }
    
    if (flag_l) {
        open_disk(disk_name, &data);
        // list_root_dir(data);
    }
}