# FAT32_recovery_tool

## Introduction
FAT32 is a widely compatible file system due to its simplicity. However, there is always a risk of accidental deletion of files. This tool is designed to recover deleted files from a FAT32 file system.

The tool works on the data stored in the FAT32 file system directly, without the OS file system support. It is capable of recovering deleted files from the root directory of the file system.

## Objectives
This tool provides a platform to learn the internals of the FAT32 file system. It is an opportunity to understand the key file system concepts and learn how to access and recover files from a raw disk. Additionally, it aims to improve the C programming skills of the user by providing an understanding of how to write code that manipulates data at the byte level.

## Usage
This tool is designed to work on a FAT32 disk image. 
```bash
./nyufile disk <options>
```
-i                     Print the file system information.\
-l                     List the root directory.\
-r filename [-s sha1]  Recover a contiguous file.\
-R filename -s sha1    Recover a possibly non-contiguous file.\


You can create a FAT32 disk image by following the steps provided below:

Create an empty file of a certain size using the dd command on Linux.
Format the disk with FAT32 using the mkfs.fat command on Linux.
Verify the file system information using the fsck.fat command on Linux.
Mount the file system using the mount command on Linux.
Play with the file system.
Once the FAT32 disk image is created and mounted, you can use this tool to recover the deleted file. The tool takes the filename of the deleted file as input and outputs the contents of the recovered file to the standard output.

Note: This tool assumes that the deleted file is in the root directory. Therefore, it does not search subdirectories for the deleted file.