/*
  
  The MEDICI Electronic Data Interchange Library
  Copyright (C) 2002  David Coles

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef MEDICI_H
#define MEDICI_H

#include "common.h"
#include "prmtrs.h"

typedef void *EDI_Directory;
typedef void *EDI_Parameters;
typedef void *EDI_Parser;
typedef void *EDI_Segment;
typedef void *EDI_Token;

typedef edi_pragma_t EDI_Pragma;
typedef edi_parameter_t EDI_Parameter;
typedef edi_data_type_t EDI_DataType;

typedef void (*EDI_ErrorHandler) (void *, int);
typedef void (*EDI_TokenHandler) (void *, EDI_Token);
typedef void (*EDI_CharacterHandler) (void *, char *, long);
/** \brief blah blah */
typedef void (*EDI_StructureHandler) (void *, edi_event_t, EDI_Parameters);
typedef void (*EDI_SegmentHandler) (void *, EDI_Parameters,
				    EDI_Segment, EDI_Directory);

typedef EDI_Directory (*EDI_DirectoryHandler) (void *, EDI_Parameters);



/* medici.c */
EDI_Parser EDI_ParserCreate (void);
EDI_Pragma EDI_SetPragma (EDI_Parser, EDI_Pragma);
EDI_StructureHandler EDI_SetStartHandler (EDI_Parser, EDI_StructureHandler);
EDI_StructureHandler EDI_SetEndHandler (EDI_Parser, EDI_StructureHandler);
EDI_ErrorHandler EDI_SetErrorHandler (EDI_Parser, EDI_ErrorHandler);
EDI_ErrorHandler EDI_SetWarningHandler (EDI_Parser, EDI_ErrorHandler);
EDI_TokenHandler EDI_SetTokenHandler (EDI_Parser, EDI_TokenHandler);
EDI_DirectoryHandler EDI_SetDirectoryHandler (EDI_Parser,
					      EDI_DirectoryHandler);
EDI_SegmentHandler EDI_SetSegmentHandler (EDI_Parser, EDI_SegmentHandler);
void *EDI_SetUserData (EDI_Parser, void *);
long EDI_Parse (EDI_Parser, char *, long, int);
int EDI_GetErrorCode (EDI_Parser);
char *EDI_GetErrorString (int);
void EDI_ParserFree (EDI_Parser);
long EDI_GetCurrentSegmentIndex (EDI_Parser);
char *EDI_GetParameter (EDI_Parameters, EDI_Parameter);
char *EDI_GetCode (EDI_Segment);
int EDI_GetElementCount (EDI_Segment);
int EDI_GetSubelementCount (EDI_Segment, int);
char *EDI_GetElement (EDI_Segment, int, int);
unsigned long EDI_GetCurrentByteIndex (EDI_Parser);
char *EDI_GetParameterString (EDI_Parameter);
char *EDI_GetElementByName (EDI_Directory, EDI_Segment, char *);
int EDI_EvaluateString (edi_data_type_t, char *, edi_data_type_t, void *);
EDI_Parameters EDI_GetParserInfo (EDI_Parser);
char *EDI_MEDICIVersion (void);
char *EDI_ElementName (EDI_Directory, char *);
char *EDI_ElementDesc (EDI_Directory, char *);
char *EDI_ElementNote (EDI_Directory, char *);
edi_item_t EDI_ElementRepresentation (EDI_Directory, char *);
char *EDI_SegmentName (EDI_Directory, char *);
char *EDI_SegmentDesc (EDI_Directory, char *);
char *EDI_SegmentNote (EDI_Directory, char *);
char *EDI_CompositeName (EDI_Directory, char *);
char *EDI_CompositeDesc (EDI_Directory, char *);
char *EDI_CompositeNote (EDI_Directory, char *);
int EDI_isComposite (EDI_Directory, char *, char *);
char EDI_SegmentReqr (EDI_Directory, char *, char *);
char EDI_CompositeReqr (EDI_Directory, char *, char *);
char *EDI_CodelistName (EDI_Directory, char *, char *);
char *EDI_CodelistDesc (EDI_Directory, char *, char *);
char *EDI_CodelistNote (EDI_Directory, char *, char *);
int EDI_ElementIndex (EDI_Directory, char *, int *, int *);
char *EDI_GetCodelistValue (EDI_Directory, char *, char *);
void EDI_DirectoryFree (EDI_Directory);

EDI_Directory EDI_GetServiceDirectory (EDI_Parser);
edi_item_t EDI_SegmentItem (EDI_Directory, char *, unsigned int);
edi_item_t EDI_CompositeItem (EDI_Directory, char *, unsigned int);

#endif /*MEDICI_H */
