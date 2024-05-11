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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "internal.h"
#include "medici.h"

/** \file medici.c

    \brief External C API

*/

/**
   \defgroup medici medici
   \brief External C API

   These are the "official" C API functions that client applications
   should use. Pointers to structures are opaque (cast to void *).

   \{
*/



/**
   \brief Construct a new parser.
   \return Opaque pointer to the new parser object.
*/
EDI_Parser
EDI_ParserCreate (void)
{
  return edi_parser_create (EDI_UNKNOWN);
}

EDI_Pragma
EDI_SetPragma (EDI_Parser p, EDI_Pragma pragma)
{
  EDI_Pragma old = ((edi_parser_t *) p)->pragma;
  ((edi_parser_t *) p)->pragma = pragma;
  return (old);
}

/**
   \brief Set handler for start of structural elements.
   \return Pointer to previously set handler.

   The type of "element" is indicated by the edi_event_t enumeration,
   and "attributes" are contained in the EDI_Parameters object.

*/
EDI_StructureHandler
EDI_SetStartHandler (EDI_Parser this, EDI_StructureHandler h)
{
  return (EDI_StructureHandler)
    edi_parser_set_start_handler (this, (edi_structure_handler_t) h);
}

/**
   \brief Set handler for end of structural elements.
 */
EDI_StructureHandler
EDI_SetEndHandler (EDI_Parser this, EDI_StructureHandler h)
{
  return (EDI_StructureHandler)
    edi_parser_set_end_handler (this, (edi_structure_handler_t) h);
}

EDI_ErrorHandler
EDI_SetErrorHandler (EDI_Parser this, EDI_ErrorHandler h)
{
  return (EDI_ErrorHandler)
    edi_parser_set_error_handler (this, (edi_error_handler_t) h);
}

EDI_ErrorHandler
EDI_SetWarningHandler (EDI_Parser this, EDI_ErrorHandler h)
{
  return (EDI_ErrorHandler)
    edi_parser_set_warning_handler (this, (edi_error_handler_t) h);
}

EDI_TokenHandler
EDI_SetTokenHandler (EDI_Parser this, EDI_TokenHandler h)
{
  return (EDI_TokenHandler)
    edi_parser_set_token_handler (this, (edi_token_handler_t) h);
}

EDI_DirectoryHandler
EDI_SetDirectoryHandler (EDI_Parser this, EDI_DirectoryHandler h)
{
  return (EDI_DirectoryHandler)
    edi_parser_set_directory_handler (this, (edi_directory_handler_t) h);
}

EDI_SegmentHandler
EDI_SetSegmentHandler (EDI_Parser this, EDI_SegmentHandler h)
{
  return (EDI_SegmentHandler)
    edi_parser_set_segment_handler (this, (edi_segment_handler_t) h);
}

void *
EDI_SetUserData (EDI_Parser p, void *v)
{
  return edi_parser_set_user_data (p, v);
}

long
EDI_Parse (EDI_Parser this, char *buffer, long length, int done)
{
  return edi_parser_parse (this, buffer, length, done);
}

int
EDI_GetErrorCode (EDI_Parser this)
{
  return edi_parser_get_error_code (this);
}

char *
EDI_GetErrorString (int error)
{
  return edi_error_string (error);
}

void
EDI_ParserFree (EDI_Parser p)
{
  edi_parser_free (p);
}


long
EDI_GetCurrentSegmentIndex (EDI_Parser p)
{
  return edi_parser_get_segment_index (p);
}

char *
EDI_GetParameter (EDI_Parameters ps, EDI_Parameter p)
{
  return edi_parameters_get (ps, p);
}

char *
EDI_GetCode (EDI_Segment s)
{
  return edi_segment_get_code (s);
}

int
EDI_GetElementCount (EDI_Segment s)
{
  return edi_segment_get_element_count (s);
}

int
EDI_GetSubelementCount (EDI_Segment s, int e)
{
  return edi_segment_get_subelement_count (s, e);
}


char *
EDI_GetElement (EDI_Segment s, int x, int y)
{
  return edi_segment_get_element (s, x, y);
}

unsigned long
EDI_GetCurrentByteIndex (EDI_Parser this)
{
  return edi_parser_get_byte_index ((edi_parser_t *) this);
}

char *
EDI_GetParameterString (EDI_Parameter p)
{
  return edi_parameters_get_string (p);
}

char *
EDI_GetElementByName (EDI_Directory d, EDI_Segment s, char *element)
{
  int x = 0, y = 0;
  char key[256], *value = NULL;

  if (!d || !s || !element)
    return value;

  sprintf (key, "%s/%s", edi_segment_get_code (s), element);

  value = edi_directory_element_index (d, key, &x, &y) ?
    edi_segment_get_element (s, x, y) : NULL;

  return value;
}

int EDI_EvaluateString
  (edi_data_type_t criteria, char *string, edi_data_type_t as, void *value)
{
  edi_evaluate_element (criteria, string, as, value);
  return 1;
}

EDI_Parameters
EDI_GetParserInfo (EDI_Parser p)
{
  return edi_parser_info ((edi_parser_t *) p);
}

char *
EDI_MEDICIVersion (void)
{
  return EDI_MEDICI_VERSION;
}



















char *
EDI_ElementName (EDI_Directory this, char *ref)
{
  return edi_directory_element_name (this, ref);
}

char *
EDI_ElementDesc (EDI_Directory this, char *ref)
{
  return edi_directory_element_desc (this, ref);
}

char *
EDI_ElementNote (EDI_Directory this, char *ref)
{
  return edi_directory_element_note (this, ref);
}

edi_item_t
EDI_ElementRepresentation (EDI_Directory this, char *ref)
{
  return edi_directory_element_repr (this, ref);
}

char *
EDI_SegmentName (EDI_Directory this, char *ref)
{
  return edi_directory_segment_name (this, ref);
}

char *
EDI_SegmentDesc (EDI_Directory this, char *ref)
{
  return edi_directory_segment_desc (this, ref);
}

char *
EDI_SegmentNote (EDI_Directory this, char *ref)
{
  return edi_directory_segment_note (this, ref);
}

char *
EDI_CompositeName (EDI_Directory this, char *ref)
{
  return edi_directory_composite_name (this, ref);
}

char *
EDI_CompositeDesc (EDI_Directory this, char *ref)
{
  return edi_directory_composite_desc (this, ref);
}

char *
EDI_CompositeNote (EDI_Directory this, char *ref)
{
  return edi_directory_composite_note (this, ref);
}

int
EDI_isComposite (EDI_Directory this, char *t, char *ref)
{
  return edi_directory_is_composite (this, t, ref);
}

char
EDI_SegmentReqr (EDI_Directory this, char *t, char *ref)
{
  return edi_directory_segment_reqr (this, t, ref);
}

char
EDI_CompositeReqr (EDI_Directory this, char *t, char *ref)
{
  return edi_directory_composite_reqr (this, t, ref);
}

char *
EDI_CodelistName (EDI_Directory this, char *c, char *v)
{
  return edi_directory_codelist_name (this, c, v);
}

char *
EDI_CodelistDesc (EDI_Directory this, char *c, char *v)
{
  return edi_directory_codelist_desc (this, c, v);
}

char *
EDI_CodelistNote (EDI_Directory this, char *c, char *v)
{
  return edi_directory_codelist_note (this, c, v);
}


int
EDI_ElementIndex (EDI_Directory this, char *key, int *x, int *y)
{
  return edi_directory_element_index (this, key, x, y);
}

char *
EDI_GetCodelistValue (EDI_Directory d, char *element, char *value)
{
  return edi_directory_codelist_value (d, element, value);
}




void
EDI_DirectoryFree (EDI_Directory d)
{
  edi_directory_free (d);
}


EDI_Directory
EDI_GetServiceDirectory (EDI_Parser this)
{
  return edi_parser_service (this);
}

edi_item_t
EDI_SegmentItem (EDI_Directory d, char *code, unsigned int n)
{
  return edi_directory_segment_item (d, code, n);
}

edi_item_t
EDI_CompositeItem (EDI_Directory d, char *code, unsigned int n)
{
  return edi_directory_composite_item (d, code, n);
}


/** \} */
