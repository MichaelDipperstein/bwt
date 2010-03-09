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
*   $Id: bwxform.h,v 1.2 2005/05/02 13:35:49 michael Exp $
*   $Log: bwxform.h,v $
*   Revision 1.2  2005/05/02 13:35:49  michael
*   Update e-mail address.
*
*   Revision 1.1.1.1  2004/08/23 04:34:18  michael
*   Burrows-Wheeler Transform
*
****************************************************************************
*
* bwxform: An ANSI C Burrows-Wheeler Transform/Reverse Transform Routines
* Copyright (C) 2004 by Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 /* Transform inFile */
int BWXformFile(char *inFile, char *outFile, char mtf);

/* Reverse Transform inFile*/
int BWReverseXformFile(char *inFile, char *outFile, char mtf);

#endif  /* ndef _BWXFORM_H_ */
