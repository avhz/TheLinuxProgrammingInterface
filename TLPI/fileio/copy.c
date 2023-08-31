/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */

/*
copy.c

Copy the file named argv[1] to a new file named in argv[2].
*/
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags; // Declare file descriptors and flags.
    mode_t filePerms;                 // Declare variable for file permissions.
    ssize_t numRead;                  // Declare the number of bytes read.
    char buf[BUF_SIZE];               // Buffer to temporarily hold data.

    // Check for correct command-line arguments.
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
        usageErr("%s old-file new-file\n", argv[0]);
    }

    // Open the source file in read-only mode.
    inputFd = open(argv[1], O_RDONLY);

    if (inputFd == -1)
    {
        errExit("opening file %s", argv[1]);
    }

    // Set flags for creating the output file and set the file permissions.
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw-rw-rw- */

    // Open the destination file with specified flags and permissions.
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)
    {
        errExit("opening file %s", argv[2]);
    }

    // Transfer data until we encounter end of input or an error
    // Loop to read from the source file and write to the destination file.
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
    {
        if (write(outputFd, buf, numRead) != numRead)
        {
            // If write fails, exit with a fatal error.
            fatal("write() returned error or partial write occurred");
        }
    }

    // Check for read error.
    if (numRead == -1)
    {
        errExit("read"); // If read operation fails, exit with an error.
    }

    // Close the source file descriptor.
    if (close(inputFd) == -1)
    {
        errExit("close input"); // If close fails, exit with an error.
    }

    // Close the destination file descriptor.
    if (close(outputFd) == -1)
    {
        errExit("close output"); // If close fails, exit with an error.
    }

    // Successfully exit the program.
    exit(EXIT_SUCCESS);
}
