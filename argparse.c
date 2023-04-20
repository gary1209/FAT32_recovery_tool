#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>

#include "argparse.h"

#define DEBUG 0

void convert_filename(unsigned char *src, char *dest) {
    char name[9];
    char ext[4];
    int i = 0;
    for (i = 0; i < 8; i++) {
        if (src[i] == ' ') break;
        name[i] = src[i];
    }
    name[i] = '\0';

    for (i = 8; i < 11; i++) {
        if (src[i] == ' ') break;
        ext[i - 8] = src[i];
    }
    ext[i - 8] = '\0';
    if (strlen(ext) > 0) {
        sprintf(dest, "%s.%s", name, ext);
    } else {
        sprintf(dest, "%s", name);
    }

}

int convert_cluster(unsigned short high, unsigned short low) {
    unsigned int clus_num = high;
    clus_num = (clus_num << 16) | low;
    return (int)clus_num;
}

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


int get_fat_entry(void *data, unsigned int clus_num) {
    BootEntry *ptr = (BootEntry *)data;
    unsigned int reserved_area = ptr->BPB_BytsPerSec * ptr->BPB_RsvdSecCnt;
    unsigned char *entry = (unsigned char*)data + reserved_area;
    int *fat = (int*) entry;
    return fat[clus_num];
}

void print_fs_info(void *data) {
    BootEntry *ptr = (BootEntry *)data;
    printf("Number of FATs = %d\n", ptr->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", ptr->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", ptr->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", ptr->BPB_RsvdSecCnt);
}


void list_root_dir(void *data) {
    BootEntry *ptr = (BootEntry *)data;
    unsigned int root_clus_num = ptr->BPB_RootClus;
    unsigned int clus_num = root_clus_num;
    int num_entries = 0;
    while (clus_num < 0x0ffffff8) {
        unsigned int reserved_area_size = ptr->BPB_BytsPerSec * ptr->BPB_RsvdSecCnt;
        unsigned int fat_area_size = ptr->BPB_NumFATs * ptr->BPB_FATSz32 * ptr->BPB_BytsPerSec; 
        unsigned char *clus_data = (unsigned char *)data + reserved_area_size + fat_area_size + ((clus_num - 2) * ptr->BPB_SecPerClus * ptr->BPB_BytsPerSec);
        if (DEBUG) {
            printf("reserved area size :%x\n", reserved_area_size);
            printf("fat area size :%x\n", fat_area_size);
            printf("clus_num :%x\n",clus_num);
            printf("clus_data: %x\n", reserved_area_size + fat_area_size + ((clus_num - 2) * ptr->BPB_SecPerClus * ptr->BPB_BytsPerSec));
        }

        for (int i = 0; i < ptr->BPB_SecPerClus * ptr->BPB_BytsPerSec; i+=32) {
            DirEntry *dir = (DirEntry *)(clus_data + i);
            if (dir->DIR_Name[0] == 0xE5 || dir->DIR_Name[0] == 0x00) {
                continue;
            }
            char filename[13];
            convert_filename(dir->DIR_Name, filename);
            int start_clus = convert_cluster(dir->DIR_FstClusHI, dir->DIR_FstClusLO);
            if (dir->DIR_Attr == 0x10) {
                // Is a directory
                printf("%s/ (starting cluster = %d)\n", filename, start_clus);
            } else if (dir->DIR_Attr == 0x0f){
                // Long file name
                perror("Should not have a long file here\n");
                ;
            } else {
                // Is a file
                if (dir->DIR_FileSize == 0) {
                    printf("%s (size = 0)\n", filename);
                } else {
                    printf("%s (size = %d, starting cluster = %d)\n", filename, dir->DIR_FileSize, start_clus);
                }
            }
            num_entries++;
        }
        clus_num = get_fat_entry(data, clus_num);
    }
    printf("Total number of entries = %d\n", num_entries);
}


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
        list_root_dir(data);
    }
}