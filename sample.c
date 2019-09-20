/***************************************************************************
*          Sample Program Using Burrows-Wheeler Transform Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of Burrows-Wheeler transform library
*   Author  : Michael Dipperstein
*   Date    : August 20, 2004
*
****************************************************************************
*
* SAMPLE: Sample usage of Burrows-Wheeler transform library
* Copyright (C) 2004-2005, 2007, 2014, 2017 by
* Michael Dipperstein (mdipperstein@gmail.com)
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
#include "optlist/optlist.h"
#include "bwxform.h"

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

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
    FILE *inFile, *outFile; /* pointer to input & output files */
    char encode;            /* encode/decode */
    xform_t method;         /* perform move to front */

    /* initialize data */
    inFile = NULL;
    outFile = NULL;
    encode = 1;
    method = XFORM_WITHOUT_MTF;

    /* parse command line */
    optList = GetOptList(argc, argv, "cdmi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'c':       /* compression mode */
                encode = 1;
                break;

            case 'd':       /* decompression mode */
                encode = 0;
                break;

            case 'm':       /* perform move to front */
                method = XFORM_WITH_MTF;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    fclose(inFile);

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((inFile = fopen(thisOpt->argument, "rb")) == NULL)
                {
                    perror("Opening Input File");

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    fclose(outFile);

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((outFile = fopen(thisOpt->argument, "wb")) == NULL)
                {
                    perror("Opening Output File");

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", FindFileName(argv[0]));
                printf("options:\n");
                printf("  -c : Encode input file to output file.\n");
                printf("  -d : Decode input file to output file.\n");
                printf("  -m : Perform the Move-to-Front coding.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -c\n", FindFileName(argv[0]));
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
        fprintf(stderr, "Enter \"%s -?\" for help.\n", FindFileName(argv[0]));

        if (outFile != NULL)
        {
            free(outFile);
        }

        exit (EXIT_FAILURE);
    }
    else if (outFile == NULL)
    {
        fprintf(stderr, "Output file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", FindFileName(argv[0]));

        if (inFile != NULL)
        {
            free(inFile);
        }

        exit (EXIT_FAILURE);
    }

    /* we have valid parameters encode or decode */
    if (encode)
    {
        BWXform(inFile, outFile, method);
    }
    else
    {
        BWReverseXform(inFile, outFile, method);
    }

    fclose(inFile);
    fclose(outFile);
    return EXIT_SUCCESS;
}
