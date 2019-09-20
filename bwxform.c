/***************************************************************************
*                    Burrows-Wheeler Transform Library
*
*   File    : bwxform.c
*   Purpose : Provides prototypes for functions that apply and reverse the
*             Burrows-Wheeler transform (with or without move to front
*             coding/decoding).  The algorithms implemented are based upon
*             those described in "A Block-sorting Lossless Data Compression
*             Algorithm" by M. Burrows and D.J. Wheeler.
*   Author  : Michael Dipperstein
*   Date    : August 20, 2004
*
****************************************************************************
*
* bwxform: An ANSI C Burrows-Wheeler Transform/Reverse Transform Routines
* Copyright (C) 2004-2005, 2007, 2014 by
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
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "bwxform.h"

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define BLOCK_SIZE  4096        /* size of blocks */

#if BLOCK_SIZE > INT_MAX
#error BLOCK_SIZE must be <= INT_MAX and maximum size_t
#endif

/* NOTE: Need to find a way to check for maximum size_t */

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
static unsigned char block[BLOCK_SIZE];     /* block being (un)transformed */
static size_t blockSize;                    /* actual size of block */

/***************************************************************************
*                                 MACROS
***************************************************************************/
/* wraps array index within array bounds (assumes value < 2 * limit) */
#define Wrap(value, limit)      (((value) < (limit)) ? (value) : ((value) - (limit)))

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
/* move to front functions */
static int DoMTF(const unsigned char *const last, const int length);
static int UndoMTF(unsigned char *const last, const int length);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : ComparePresorted
*   Description: This comparison function is designed for use with qsort
*                and "block", a global array of "blockSize" unsigned chars.
*                It compares two strings in "block" starting at indices
*                s1 and s2 and ending at indices s1 - 1 and s2 - 1.
*                The strings are assumed to be presorted so that first two
*                characters are known to be matching.
*   Parameters : s1 - The starting index of a string in block
*                s2 - The starting index of a string in block
*   Effects    : NONE
*   Returned   : > 0 if string s1 > string s2
*                0 if string s1 == string s2
*                < 0 if string s1 < string s2
***************************************************************************/
static int ComparePresorted(const void *s1, const void *s2)
{
    unsigned int offset1, offset2;
    unsigned int i;

    /***********************************************************************
    * Compare 1 character at a time until there's difference or the end of
    * the block is reached.  Since we're only sorting strings that already
    * match at the first two characters, start with the third character.
    ***********************************************************************/
    offset1 = *((unsigned int *)s1) + 2;
    offset2 = *((unsigned int *)s2) + 2;

    for(i = 2; i < blockSize; i++)
    {
        unsigned char c1, c2;

        /* ensure that offsets are properly bounded */
        if (offset1 >= blockSize)
        {
            offset1 -= blockSize;
        }

        if (offset2 >= blockSize)
        {
            offset2 -= blockSize;
        }

        c1 = block[offset1];
        c2 = block[offset2];

        if (c1 > c2)
        {
            return 1;
        }
        else if (c2 > c1)
        {
            return -1;
        }

        /* strings match to here, try next character */
        offset1++;
        offset2++;
    }

    /* strings are identical */
    return 0;
}

/***************************************************************************
*   Function   : BWXform
*   Description: This function performs a Burrows-Wheeler transformation
*                on a file (with optional move to front) and writes the
*                resulting data to the specified output file.  Comments in
*                this function indicate corresponding variables, labels,
*                and sections in "A Block-sorting Lossless Data Compression
*                Algorithm" by M. Burrows and D.J. Wheeler.
*   Parameters : fpIn - FILE pointer to file to transform
*                fpOut - FILE pointer to file to write transformed output
*                method - Set to XFORM_WITH_MTF if move to front coding
*                      should be applied.
*   Effects    : A Burrows-Wheeler transformation (and possibly move to
*                front encoding) is applied to fpIn.   The results of
*                the transformation are written to fpOut.
*   Returned   : Zero for success, otherwise non-zero.
***************************************************************************/
int BWXform(FILE *fpIn, FILE *fpOut, const xform_t method)
{
    unsigned int i, j, k;
    unsigned int *rotationIdx;      /* index of first char in rotation */
    unsigned int *v;                /* index of radix sorted charaters */
    int s0Idx;                      /* index of S0 in rotations (I) */
    unsigned char *last;            /* last characters from sorted rotations */

    /* counters and offsets used for radix sorting with characters */
    unsigned int counters[256];
    unsigned int offsetTable[256];

    if ((NULL == fpIn) || (NULL == fpOut))
    {
        fprintf(stderr, "Invalid File Pointer Arguments\n");
        return -1;
    }

    /***********************************************************************
    * BLOCK_SIZE arrays are allocated on the heap, because gcc generates
    * code that throws a Segmentation fault when the large arrays are
    * allocated on the stack.
    ***********************************************************************/
    rotationIdx = (unsigned int *)malloc(BLOCK_SIZE * sizeof(unsigned int));

    if (NULL == rotationIdx)
    {
        perror("Allocating array of rotation indices");
        return errno;
    }

    v = (unsigned int *)malloc(BLOCK_SIZE * sizeof(unsigned int));

    if (v == rotationIdx)
    {
        perror("Allocating array of sort indices");
        free(rotationIdx);
        return errno;
    }

    last = (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));

    if (NULL == last)
    {
        perror("Allocating array of last characters");
        free(rotationIdx);
        free(v);
        return errno;
    }

    while((blockSize = fread(block, sizeof(unsigned char), BLOCK_SIZE, fpIn))
        != 0)
    {
        /*******************************************************************
        * Sort the rotated strings in the block.  A radix sort is performed
        * on the first to characters of all the rotated strings (2nd
        * character then 1st).  All rotated strings with matching initial
        * characters are then quicksorted. - Q4..Q7
        *******************************************************************/

        /*** radix sort on second character in rotation ***/

        /* count number of characters for radix sort */
        memset(counters, 0, 256 * sizeof(int));
        for (i = 0; i < blockSize; i++)
        {
            counters[block[i]]++;
        }

        offsetTable[0] = 0;

        for(i = 1; i < 256; i++)
        {
            /* determine number of values before those sorted under i */
            offsetTable[i] = offsetTable[i - 1] + counters[i - 1];
        }

        /* sort on 2nd character */
        for (i = 0; i < blockSize - 1; i++)
        {
            j = block[i + 1];
            v[offsetTable[j]] = i;
            offsetTable[j] = offsetTable[j] + 1;
        }

        /* handle wrap around for string starting at end of block */
        j = block[0];
        v[offsetTable[j]] = i;
        offsetTable[0] = 0;

        /*** radix sort on first character in rotation ***/

        for(i = 1; i < 256; i++)
        {
            /* determine number of values before those sorted under i */
            offsetTable[i] = offsetTable[i - 1] + counters[i - 1];
        }

        for (i = 0; i < blockSize; i++)
        {
            j = v[i];
            j = block[j];
            rotationIdx[offsetTable[j]] = v[i];
            offsetTable[j] = offsetTable[j] + 1;
        }

        /*******************************************************************
        * now rotationIdx contains the sort order of all strings sorted
        * by their first 2 characters.  Use qsort to sort the strings
        * that have their first two characters matching.
        *******************************************************************/
        for (i = 0, k = 0; (i <= UCHAR_MAX) && (k < (blockSize - 1)); i++)
        {
            for (j = 0; (j <= UCHAR_MAX) && (k < (blockSize - 1)); j++)
            {
                unsigned int first = k;

                /* count strings starting with ij */
                while ((i == block[rotationIdx[k]]) &&
                    (j == block[Wrap(rotationIdx[k] + 1,  blockSize)]))
                {
                    k++;

                    if (k == blockSize)
                    {
                        /* we've searched the whole block */
                        break;
                    }
                }

                if (k - first > 1)
                {
                    /* there are at least 2 strings staring with ij, sort them */
                    qsort(&rotationIdx[first], k - first, sizeof(int),
                        ComparePresorted);
                }
            }
        }

        /* find last characters of rotations (L) - C2 */
        s0Idx = 0;
        for (i = 0; i < blockSize; i++)
        {
            if (rotationIdx[i] != 0)
            {
                last[i] = block[rotationIdx[i] - 1];
            }
            else
            {
                /* unrotated string 1st character is end of string */
                s0Idx = i;
                last[i] = block[blockSize - 1];
            }
        }

        if (XFORM_WITH_MTF == method)
        {
            int ret;

            ret = DoMTF(last, blockSize);

            if (ret)
            {
                free(rotationIdx);
                free(v);
                free(last);
                return errno;
            }
        }

        /* write index of end of unrotated string (I) */
        fwrite(&s0Idx, sizeof(int), 1, fpOut);

        /* write out last characters of rotations (L) */
        fwrite(last, sizeof(unsigned char), blockSize, fpOut);
    }

    /* clean up */
    free(rotationIdx);
    free(v);
    free(last);
    return 0;
}

/***************************************************************************
*   Function   : DoMTF
*   Description: This function performs move to front encoding on a block
*                on of data that has already had the Burrows-Wheeler
*                transformation applied to it.  Comments in this function
*                indicate corresponding variables, labels, and sections in
*                "A Block-sorting Lossless Data Compression Algorithm" by
*                M. Burrows and D.J. Wheeler.
*   Parameters : last - pointer an array of "last" characters from
*                       Burrows-Wheeler rotations (L)
*                length - the number of unsigned chars contained in last.
*   Effects    : Move to front encoding is applied on an array of last
*                characters.  The results of the encoding replace the data
*                that was stored in last.
*   Returned   : Zero for success, otherwise non-zero.
***************************************************************************/
static int DoMTF(const unsigned char *const last, const int length)
{
    unsigned char list[UCHAR_MAX + 1];      /* list of characters (Y) */
    unsigned char *encoded;                 /* mtf encoded block (R) */
    int i, j;

    /***********************************************************************
    * BLOCK_SIZE arrays are allocated on the heap, because gcc generates
    * code that throws a Segmentation fault when the large arrays are
    * allocated on the stack.
    ***********************************************************************/
    encoded = (unsigned char *)malloc(length * sizeof(unsigned char));

    if (NULL == encoded)
    {
        perror("Allocating array to store MTF encoding");
        return errno;
    }

    /* start with alphabetically sorted list of characters */
    for(i = 0; i <= UCHAR_MAX; i++)
    {
        list[i] = (unsigned char)i;
    }

    /* move-to-front coding - M1 */
    for (i = 0; i < length; i++)
    {
        /*******************************************************************
        * Find the character in the list of characters.  I do a sequential
        * search because move to front causes common characters to be
        * near the front of the list.
        *******************************************************************/
        for (j = 0; j <= UCHAR_MAX; j++)
        {
            if (list[j] == last[i])
            {
                /* we found the character */
                encoded[i] = j;
                break;
            }
        }

        /* now move the current character to the front of the list */
        memmove(&(list[1]), list, j);
        list[0] = last[i];
    }

    /* copy mtf encoded vector of last characters (R) to input */
    memcpy((void *)last, (void *)encoded, sizeof(unsigned char) * length);
    free(encoded);

    return 0;
}

/***************************************************************************
*   Function   : BWReverseXformFile
*   Description: This function reverses a Burrows-Wheeler transformation
*                on a file (with optional move to front) and writes the
*                resulting data to the specified output file.  Comments in
*                this function indicate corresponding variables, labels,
*                and sections in "A Block-sorting Lossless Data Compression
*                Algorithm" by M. Burrows and D.J. Wheeler.
*   Parameters : fpIn - FILE pointer to file to reverse transform
*                fpOut - FILE pointer to file to write reverse transformed
*                          output to
*                method - Set to XFORM_WITH_MTF if move to front coding
*                      should be applied.
*   Effects    : A Burrows-Wheeler reverse transformation (and possibly
*                move to front encoding) is applied to fpIn.   The results
*                of the reverse transformation are written to fpOut.
*   Returned   : Zero for success, otherwise non-zero.
***************************************************************************/
int BWReverseXform(FILE *fpIn, FILE *fpOut, const xform_t method)
{
    unsigned int i, j, sum;
    int count[UCHAR_MAX + 1];   /* count[i] = # of chars in block <= i */
    int *pred;                  /* pred[i] = # of times block[i] appears in
                                   block[0 .. i - 1] */
    unsigned char *unrotated;   /* original block */
    int s0Idx;                  /* index of S0 in rotations (I) */

    if ((NULL == fpIn) || (NULL == fpOut))
    {
        fprintf(stderr, "Invalid File Pointer Arguments\n");
        return -1;
    }

    /***********************************************************************
    * BLOCK_SIZE arrays are allocated on the heap, because gcc generates
    * code that throws a Segmentation fault when the large arrays are
    * allocated on the stack.
    ***********************************************************************/
    pred = (int *)malloc(BLOCK_SIZE * sizeof(int));

    if (NULL == pred)
    {
        perror("Allocating array of matching predecessors");
        return errno;
    }

    unrotated = (unsigned char *)malloc(BLOCK_SIZE * sizeof(unsigned char));

    if (NULL == unrotated)
    {
        perror("Allocating array to store unrotated block");
        free(pred);
        return errno;
    }

    while(fread(&s0Idx, sizeof(int), 1, fpIn) != 0)
    {
        blockSize = fread(block, sizeof(unsigned char), BLOCK_SIZE, fpIn);

        if (XFORM_WITH_MTF == method)
        {
            int ret;

            ret = UndoMTF(block, blockSize);

            if (ret)
            {
                free(pred);
                free(unrotated);
                return ret;
            }
        }

        /* code based on pseudo code from section 4.2 (D1 and D2) follows */
        for(i = 0; i <= UCHAR_MAX; i++)
        {
            count[i] = 0;
        }

        /*******************************************************************
        * Set pred[i] to the number of times block[i] appears in the
        * substring block[0 .. i - 1].  As a useful side effect count[i]
        * will be the number of times character i appears in block.
        *******************************************************************/
        for (i = 0; i < blockSize; i++)
        {
            pred[i] = count[block[i]];
            count[block[i]]++;
        }

        /*******************************************************************
        * Finally, set count[i] to the number of characters in block
        * lexicographically less than i.
        *******************************************************************/
        sum = 0;
        for(i = 0; i <= UCHAR_MAX; i++)
        {
            j = count[i];
            count[i] = sum;
            sum += j;
        }

        /* construct the initial unrotated string (S[0]) */
        i = s0Idx;
        for(j = blockSize; j > 0; j--)
        {
            unrotated[j - 1] = block[i];
            i = pred[i] + count[block[i]];
        }

        fwrite(unrotated, sizeof(unsigned char), blockSize, fpOut);
    }

    /* clean up */
    free(pred);
    free(unrotated);
    return 0;
}

/***************************************************************************
*   Function   : UndoMTF
*   Description: This function reverses move to front encoding on a block
*                on of data that has already had the Burrows-Wheeler
*                transformation applied to it.  Comments in this function
*                indicate corresponding variables, labels, and sections in
*                "A Block-sorting Lossless Data Compression Algorithm" by
*                M. Burrows and D.J. Wheeler.
*   Parameters : last - pointer an array of mtf encoded characters from
*                       Burrows-Wheeler rotations.
*                length - the number of unsigned chars contained in last.
*   Effects    : Move to front encoding is reversed on an array of last
*                characters.  The results of the reversal are stored in
*                the array last (L), providing an array of last characters
*                of sorted rotations.
*   Returned   : Zero for success, otherwise non-zero.
***************************************************************************/
static int UndoMTF(unsigned char *const last, const int length)
{
    unsigned char list[UCHAR_MAX + 1];      /* list of characters (Y) */
    unsigned char *encoded;                 /* mtf encoded block (R) */
    int i;

    /***********************************************************************
    * BLOCK_SIZE arrays are allocated on the heap, because gcc generates
    * code that throws a Segmentation fault when the large arrays are
    * allocated on the stack.
    ***********************************************************************/
    encoded = (unsigned char *)malloc(length * sizeof(unsigned char));

    if (NULL == encoded)
    {
        perror("Allocating array to store MTF encoding");
        return errno;
    }

    /* copy last into encoded */
    memcpy((void *)encoded, (void *)last, sizeof(unsigned char) * length);

    /* start with alphabetically sorted list of characters */
    for(i = 0; i <= UCHAR_MAX; i++)
    {
        list[i] = (unsigned char)i;
    }

    /* move-to-front decoding - W2 */
    for (i = 0; i < length; i++)
    {
        /* decode the character */
        last[i] = list[encoded[i]];

        /* now move the current character to the front of the list */
        memmove(&(list[1]), list, encoded[i]);
        list[0] = last[i];
    }

    free(encoded);
    return 0;
}
