#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

void print_usage() {
    printf("Usage: ./nyufile disk <options>\n");
    printf("  -i                     Print the file system information.\n");
    printf("  -l                     List the root directory.\n");
    printf("  -r filename [-s sha1]  Recover a contiguous file.\n");
    printf("  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
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

    if (!flag_i && !flag_l && !flag_r && !flag_R && !flag_s) {
        print_usage();
        exit(1);
    }

    if (flag_i && (flag_l || flag_r || flag_R || flag_s)){
        print_usage();
        exit(1);
    }

    if (flag_l && (flag_r || flag_R || flag_s)){
        print_usage();
        exit(1);
    }

    if (flag_s && !flag_r && !flag_R) {
        print_usage();
        exit(1);
    }

    if (!flag_s && flag_R) {
        print_usage();
        exit(1);
    }
}