/***************************************************************************
*               Header for Burrows-Wheeler Transform Library
*
*   File    : bwxform.h
*   Purpose : Provides that apply and reverse the Burrows-Wheeler transform
*             (with or without move to front coding/decoding.
*             transformation).
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

#ifndef _BWXFORM_H_
#define _BWXFORM_H_

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
typedef enum
{
    XFORM_WITHOUT_MTF = 0,
    XFORM_WITH_MTF = 1
} xform_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
* Transform/Reverse Transform file stream fpIn writing results to fpOut.
* Use method to indicate whether or not to use MTF.
* Zero is returned on success.
***************************************************************************/
/* Transform/Reverse Tran fpIn save results to fpOut.  Use MTF if mtf is TRUE */
int BWXform(FILE *fpIn, FILE *fpOut, const xform_t method);
int BWReverseXform(FILE *fpIn, FILE *fpOut, const xform_t method);

#endif  /* ndef _BWXFORM_H_ */
