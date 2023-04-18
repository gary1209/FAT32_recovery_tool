#include <stdio.h>

#include "argparse.h"

int main(int argc, char *argv[]) {
    char disk_name[256];
    parse_args(argc, argv, disk_name);
    return 0;
}