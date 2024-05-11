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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "internal.h"



/** \file drctry.c

    \brief The base directory (tsg) class

    The base directory class (edi_directory_t) holds the transaction
    set guidelines for a number of messages and state information for
    parsing the structure of a message.

*/

/**
   \defgroup edi_directory edi_directory
   \{
*/


/**
   \brief Start parsing the stucture of a transaction with the initial segment

   \param this directory to use
   \param segment the first segment in the transaction
   \param parameters parameters to pass to the client application
   \param parser parser object used by the client application
   \param service "service" directory used by the syntax of the message
   \param transaction name of the transaction (eg. ORDERS)

   Initialise state for parsing the structure of a transaction
*/

edi_error_t edi_directory_head
  (edi_directory_t * this,
   edi_segment_t * segment,
   edi_parameters_t * parameters,
   edi_parser_t * parser, edi_directory_t * service, char *transaction)
{
  if (!this || !this->start)
    {
      edi_parser_handle_segment (parser, parameters, service);
      return EDI_ENONE;
    }

  this->start (this, transaction);

  if (this->parse)
    return this->parse (this, segment, parameters, parser, this);
  else
    return EDI_ENONE;
}


/**
   \brief Continue parsing the stucture of a transaction with the
   next segment

   \param this directory to use
   \param segment the next segment in the transaction
   \param parameters parameters to pass to the client application
   \param parser parser object used by the client application
   \param service "service" directory used by the syntax of the message

*/

edi_error_t edi_directory_body
  (edi_directory_t * this,
   edi_segment_t * segment,
   edi_parameters_t * parameters,
   edi_parser_t * parser, edi_directory_t * service)
{
  if (!this || !this->parse)
    {
      edi_parser_handle_segment (parser, parameters, this ? this : service);
      return EDI_ENONE;
    }

  return this->parse (this, segment, parameters, parser,
		      service ? service : NULL);
}


/**
   \brief Finish parsing the stucture of a transaction with the final segment
     
   \param this directory to use
   \param segment the final segment in the transaction
   \param parameters parameters to pass to the client application
   \param parser parser object used by the client application
   \param service "service" directory used by the syntax of the message
*/

edi_error_t edi_directory_tail
  (edi_directory_t * this,
   edi_segment_t * segment,
   edi_parameters_t * parameters,
   edi_parser_t * parser, edi_directory_t * service)
{
  if (!this || !this->parse)
    {
      edi_parser_handle_segment (parser, parameters, service);
      return EDI_ENONE;
    }

  this->parse (this, segment, parameters, parser, service);

  if (this->end)
    return this->end (this);
  else
    return EDI_ENONE;
}


void
edi_directory_free (edi_directory_t * this)
{
  if (this && this->free)
    this->free (this);
}


int
edi_directory_element_index (edi_directory_t * this, char *key, int *x,
			     int *y)
{
  int n;
  char code[256], *element = NULL, *subelement = NULL;

  strncpy (code, key, 256);
  code[255] = '\0';

  for (n = 0; code[n] != '\0'; n++)
    if (code[n] == '/')
      {
	code[n] = '\0';
	if (element)
	  subelement = code + n + 1;
	else
	  element = code + n + 1;
      }

  return this->element_indx ?
    this->element_indx (this, code, element, subelement, x, y) : -1;
}


char *edi_directory_codelist_value
  (edi_directory_t * this, char *element, char *value)
{
  if (!this || !element || !value)
    return NULL;

  return edi_directory_codelist_name (this, element, value);
}

















char *
edi_directory_element_name (edi_directory_t * this, char *ref)
{
  return this && this->element_name ? this->element_name (this, ref) : NULL;
}

char *
edi_directory_element_desc (edi_directory_t * this, char *ref)
{
  return this && this->element_desc ? this->element_desc (this, ref) : NULL;
}

char *
edi_directory_element_note (edi_directory_t * this, char *ref)
{
  return this && this->element_note ? this->element_note (this, ref) : NULL;
}

char *
edi_directory_segment_name (edi_directory_t * this, char *ref)
{
  return this && this->segment_name ? this->segment_name (this, ref) : NULL;
}

char *
edi_directory_segment_desc (edi_directory_t * this, char *ref)
{
  return this && this->segment_desc ? this->segment_desc (this, ref) : NULL;
}

char *
edi_directory_segment_note (edi_directory_t * this, char *ref)
{
  return this && this->segment_note ? this->segment_note (this, ref) : NULL;
}

char *
edi_directory_composite_name (edi_directory_t * this, char *ref)
{
  return this
    && this->composite_name ? this->composite_name (this, ref) : NULL;
}

char *
edi_directory_composite_desc (edi_directory_t * this, char *ref)
{
  return this
    && this->composite_desc ? this->composite_desc (this, ref) : NULL;
}

char *
edi_directory_composite_note (edi_directory_t * this, char *ref)
{
  return this
    && this->composite_note ? this->composite_note (this, ref) : NULL;
}

int
edi_directory_is_composite (edi_directory_t * this, char *t, char *ref)
{
  return this && this->is_composite ? this->is_composite (this, t, ref) : 0;
}

char
edi_directory_segment_reqr (edi_directory_t * this, char *t, char *ref)
{
  return this && this->segment_reqr ? this->segment_reqr (this, t, ref) : 'C';
}

char
edi_directory_composite_reqr (edi_directory_t * this, char *t, char *ref)
{
  return this && this->composite_reqr ?
    this->composite_reqr (this, t, ref) : 'C';
}

char *
edi_directory_codelist_name (edi_directory_t * this, char *c, char *v)
{
  return this
    && this->codelist_name ? this->codelist_name (this, c, v) : NULL;
}

char *
edi_directory_codelist_desc (edi_directory_t * this, char *c, char *v)
{
  return this
    && this->codelist_desc ? this->codelist_desc (this, c, v) : NULL;
}

char *
edi_directory_codelist_note (edi_directory_t * this, char *c, char *v)
{
  return this
    && this->codelist_note ? this->codelist_note (this, c, v) : NULL;
}


unsigned int
edi_directory_segment_size (edi_directory_t * this, char *code)
{
  return this && this->segment_size ? this->segment_size (this, code) : 0;
}

unsigned int
edi_directory_composite_size (edi_directory_t * this, char *code)
{
  return this && this->composite_size ? this->composite_size (this, code) : 0;
}


edi_item_t edi_directory_composite_item
  (edi_directory_t * this, char *code, unsigned int i)
{
  edi_item_t item = EDI_NULL_ITEM;
  return this && this->composite_item ?
    this->composite_item (this, code, i) : item;
}

edi_item_t edi_directory_segment_item
  (edi_directory_t * this, char *code, unsigned int i)
{
  edi_item_t item = EDI_NULL_ITEM;
  return this && this->segment_item ?
    this->segment_item (this, code, i) : item;
}


edi_item_t edi_directory_element_repr (edi_directory_t * this, char *code)
{
  edi_item_t item = EDI_NULL_ITEM;
  return this && this->element_repr ? this->element_repr (this, code) : item;
}







char *edi_get_element_by_name
  (edi_directory_t * directory, edi_segment_t * segment,
   char *element, char *subelement)
{
  edi_item_t item;
  unsigned int ss, cs, e, s;
  char *tag = edi_segment_get_code (segment);

  if (!(ss = edi_directory_segment_size (directory, tag)))
    return NULL;

  for (e = 0; e < ss; e++)
    {
      item = edi_directory_segment_item (directory, tag, e);
      if (item.code && !strcmp (item.code, element))
	goto found_element;
    }

  return NULL;

found_element:

  /* is this a simple element, or composite? */
  if (!item.type)
    return edi_segment_get_element (segment, e, 0);

  if (!(cs = edi_directory_composite_size (directory, element)))
    return NULL;

  for (s = 0; s < cs; s++)
    {
      item = edi_directory_composite_item (directory, element, s);
      if (item.code && !strcmp (item.code, subelement))
	goto found_subelement;
    }

  return NULL;

found_subelement:
  return edi_segment_get_element (segment, e, s);
}

/** \} */
