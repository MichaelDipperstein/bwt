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
*   UPDATES
*
*   $Id: bwxform.h,v 1.3 2007/09/17 13:21:19 michael Exp $
*   $Log: bwxform.h,v $
*   Revision 1.3  2007/09/17 13:21:19  michael
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
* bwxform: An ANSI C Burrows-Wheeler Transform/Reverse Transform Routines
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

#ifndef _BWXFORM_H_
#define _BWXFORM_H_

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
 /* Transform fpIn save results to fpOut.  Use MTF if mtf is TRUE */
int BWXform(FILE *fpIn, FILE *fpOut, char mtf);

/* Reverse Transform fpIn  save results to fpOut.  Use MTF if mtf is TRUE */
int BWReverseXform(FILE *fpIn, FILE *fpOut, char mtf);

#endif  /* ndef _BWXFORM_H_ */
