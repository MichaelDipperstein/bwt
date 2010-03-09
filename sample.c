/***************************************************************************
*          Sample Program Using Burrows-Wheeler Transform Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of Burrows-Wheeler transform library
*   Author  : Michael Dipperstein
*   Date    : August 20, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: sample.c,v 1.3 2007/09/17 13:21:48 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.3  2007/09/17 13:21:48  michael
*   Replace getopt with optlist.
*   Changes required for LGPL v3.
*
*   Revision 1.2  2005/05/02 13:35:49  michael
*   Update e-mail address.
*
*   Revision 1.1.1.1  2004/08/23 04:34:18  michael
*   Burrows-Wheeler Transform
*
****************************************************************************
*
* SAMPLE: Sample usage of Burrows-Wheeler transform library
* Copyright (C) 2004-2005, 2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the BWT library.
*
* The BWT library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* The BWT library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optlist.h"
#include "bwxform.h"

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
char *RemovePath(char *fullPath);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will call
*                functions to transform or reverse transform a file using
*                the Burrows-Wheeler trasnform algorithm with or without
*                additional move to front coding.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes/Decodes input file
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
***************************************************************************/
int main(int argc, char *argv[])
{
    option_t *optList, *thisOpt;
    char *inFile, *outFile; /* name of input & output files */
    char encode;            /* encode/decode */
    char mtf;               /* perform move to front */

    /* initialize data */
    inFile = NULL;
    outFile = NULL;
    encode = TRUE;
    mtf = FALSE;

    /* parse command line */
    optList = GetOptList(argc, argv, "cdmi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'c':       /* compression mode */
                encode = TRUE;
                break;

            case 'd':       /* decompression mode */
                encode = FALSE;
                break;

            case 'm':       /* perform move to front */
                mtf = TRUE;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    free(inFile);

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((inFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                strcpy(inFile, thisOpt->argument);
                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    free(outFile);

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((outFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                strcpy(outFile, thisOpt->argument);
                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", RemovePath(argv[0]));
                printf("options:\n");
                printf("  -c : Encode input file to output file.\n");
                printf("  -d : Decode input file to output file.\n");
                printf("  -m : Perform the Move-to-Front coding.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -c\n", RemovePath(argv[0]));
                FreeOptList(optList);
                return(EXIT_SUCCESS);
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* validate command line */
    if (inFile == NULL)
    {
        fprintf(stderr, "Input file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", RemovePath(argv[0]));

        if (outFile != NULL)
        {
            free(outFile);
        }

        exit (EXIT_FAILURE);
    }
    else if (outFile == NULL)
    {
        fprintf(stderr, "Output file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", RemovePath(argv[0]));

        if (inFile != NULL)
        {
            free(inFile);
        }

        exit (EXIT_FAILURE);
    }

    /* we have valid parameters encode or decode */
    if (encode)
    {
        BWXformFile(inFile, outFile, mtf);
    }
    else
    {
        BWReverseXformFile(inFile, outFile, mtf);
    }

    free(inFile);
    free(outFile);
    return EXIT_SUCCESS;
}

/***************************************************************************
*   Function   : RemovePath
*   Description: This is function accepts a pointer to the name of a file
*                along with path information and returns a pointer to the
*                character that is not part of the path.
*   Parameters : fullPath - pointer to an array of characters containing
*                           a file name and possible path modifiers.
*   Effects    : None
*   Returned   : Returns a pointer to the first character after any path
*                information.
***************************************************************************/
char *RemovePath(char *fullPath)
{
    int i;
    char *start, *tmp;                          /* start of file name */
    const char delim[3] = {'\\', '/', ':'};     /* path deliminators */

    start = fullPath;

    /* find the first character after all file path delimiters */
    for (i = 0; i < 3; i++)
    {
        tmp = strrchr(start, delim[i]);

        if (tmp != NULL)
        {
            start = tmp + 1;
        }
    }

    return start;
}
