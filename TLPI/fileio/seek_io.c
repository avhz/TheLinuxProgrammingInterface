/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-3 */

/* seek_io.c

   Demonstrate the use of lseek() and file I/O system calls.

   Usage: seek_io file {r<length>|R<length>|w<string>|s<offset>}...

   This program opens the file named on its command line, and then performs
   the file I/O operations specified by its remaining command-line arguments:

           r<length>    Read 'length' bytes from the file at current
                        file offset, displaying them as text.

           R<length>    Read 'length' bytes from the file at current
                        file offset, displaying them in hex.

           w<string>    Write 'string' at current file offset.

           s<offset>    Set the file offset to 'offset'.

   Example:

        seek_io myfile wxyz s1 r2
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    size_t len;                  // To store the length for reading/writing
    off_t offset;                // To store the file offset
    int fd, ap, j;               // File descriptor, argument pointer, loop counter
    unsigned char *buf;          // Buffer to read into
    ssize_t numRead, numWritten; // Number of bytes read or written

    // Check the command-line arguments
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);
    }

    // Open (or create) the file
    /* rw-rw-rw- */
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1)
    {
        errExit("open");
    }

    // Loop through all command-line arguments starting from the 3rd
    for (ap = 2; ap < argc; ap++)
    {
        switch (argv[ap][0])
        {
        case 'r': /* Display bytes at current offset, as text */
        case 'R': /* Display bytes at current offset, in hexadecimal */
            len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);

            buf = malloc(len);
            if (buf == NULL)
            {
                errExit("malloc");
            }

            numRead = read(fd, buf, len);
            if (numRead == -1)
            {
                errExit("read");
            }

            // Display the read data
            if (numRead == 0)
            {
                printf("%s: end-of-file\n", argv[ap]);
            }
            else
            {
                printf("%s: ", argv[ap]);

                for (j = 0; j < numRead; j++)
                {
                    if (argv[ap][0] == 'r')
                    {
                        printf("%c", isprint(buf[j]) ? buf[j] : '?');
                    }
                    else
                    {
                        printf("%02x ", buf[j]);
                    }
                }
                printf("\n");
            }

            free(buf);
            break;

        // Write the string to the file
        case 'w': /* Write string at current offset */
            numWritten = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (numWritten == -1)
            {
                errExit("write");
            }
            printf("%s: wrote %ld bytes\n", argv[ap], (long)numWritten);
            break;

        // Seek to the specified offset
        case 's': /* Change file offset */
            offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            if (lseek(fd, offset, SEEK_SET) == -1)
            {
                errExit("lseek");
            }

            printf("%s: seek succeeded\n", argv[ap]);
            break;

        // Invalid command
        default:
            cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }

    // Close the file
    if (close(fd) == -1)
    {
        errExit("close");
    }

    exit(EXIT_SUCCESS);
}
