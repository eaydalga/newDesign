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

/** \file parser.c

    \brief The base parser class

    The base parser class (edi_parser_t) holds a deal of state
    information for the parse run.

*/

/**
   \defgroup edi_parser edi_parser
   \{
*/



/**
   \brief Initialises a freshly allocated parser structure.
   \param this pointer to the edi_parser_s structure to initialise.

   Mainly internal function (to be run by edi_parser_create) to
   initialise flags, counters, function pointers and ADTs.

*/

void
edi_parser_init (edi_parser_t * this)
{
  memset (this, 0, sizeof (edi_parser_t));	/* mitigate bugs */
  this->segment = edi_segment_create ();
  this->de = 0;
  this->cde = 0;
  this->segment_count = 0;
  this->error = EDI_ENONE;
  this->pragma = EDI_PCHARSET | EDI_PSEGMENT;

  edi_buffer_init (&(this->parse_buffer));
  edi_stack_init (&(this->stack));
  edi_advice_init (&(this->advice));

  this->interchange_type = EDI_UNKNOWN;
  this->syntax_fini = NULL;
  this->sgmnt_handler = NULL;

  this->handler_arg = 0;

  edi_parser_set_token_handler (this, NULL);
  edi_parser_set_error_handler (this, NULL);
  edi_parser_set_warning_handler (this, NULL);
  edi_parser_set_user_data (this, NULL);
  edi_parser_set_directory_handler (this, NULL);
  edi_parser_set_segment_handler (this, NULL);
  edi_parser_set_start_handler (this, NULL);
  edi_parser_set_end_handler (this, NULL);

  edi_list_init (&(this->token_queue));
  edi_tokeniser_init (&(this->tokeniser));

  this->tokeniser.user_data = this;
  this->tokeniser.itype_handler = edi_parser_itype_handler;
  this->tokeniser.token_handler = edi_parser_token_handler;
  this->tokeniser.error_handler =
    (edi_error_handler_t) edi_parser_raise_error;
}












/**********************************************************************
 * User-level functions for creating, parsing and freeing
 **********************************************************************/

/**
   \brief Creates a parser structure and returns a pointer to it.
   \param type Interchange type if known, otherwise EDI_ANY.
   \return Pointer to the new parser structure, or NULL on failure.

   The type should be EDI_ANY to create a parser which will accept any
   of the implemented syntaxes. Using a value other than EDI_ANY
   (eg. EDI_X12) will create a parser which will only parse a stream
   of that type - streams with other syntaxes, though valid and
   implemented will cause a parse failure.

   If a NULL value is returned this will almost certainly be because
   of a failure to allocate memory; you should consult errno(3) or
   whatever your platform uses for details.

*/

edi_parser_t *
edi_parser_create (edi_interchange_type_t type)
{
  edi_parser_t *this;

  if (!(this = malloc (sizeof (edi_parser_t))))
    return NULL;

  edi_parser_init (this);
  edi_parser_itype_handler (this, type);

  return this;
}


/**
   \brief Frees all resources allocated to this parser.
   \param this Pointer to the parser to be freed.
*/

void
edi_parser_free (edi_parser_t * this)
{
  if (!this)
    return;

  if (this->syntax_fini)
    this->syntax_fini (this);

  edi_buffer_clear (&(this->parse_buffer));
  edi_stack_clear (&(this->stack), free);

  if (this->segment)
    edi_segment_free (this->segment);

  free (this);
}


/**
   \brief Parse a chunk of an EDI stream,
   \param this Pointer to the parser which should parse the chunk.
   \param buffer Pointer to the buffer containing the chunk.
   \param length Length in characters of the chunk.
   \param done This should be set to non-zero if this is know to be the
   last chunk in the stream (zero otherwise).
   \return Number of characters consumed by the parser.

   If the number of characters consumed by the parser is less than the
   size of the chunk passed in then either the parser encountered an
   error and aborted or the interchange parsed successfully and there
   is trailing garbage (eg. another interchange concatenated on the
   end).
*/

long
edi_parser_parse (edi_parser_t * this, char *buffer, long length, int done)
{
  return edi_tokeniser_parse (&(this->tokeniser), buffer, length, done);
}











/**********************************************************************
 * Some statistical/informational reporting functions
 **********************************************************************/

/** \brief Returns the byte offset of the position in the stream. */
unsigned long
edi_parser_get_byte_index (edi_parser_t * this)
{
  return this ? this->tokeniser.byte_count : 0;
}

/** \brief Returns the segment index of the position in the stream. */
unsigned long
edi_parser_get_segment_index (edi_parser_t * this)
{
  return this ? this->segment_count : 0;
}

/** \brief Returns the type of error that has occured. */
int
edi_parser_get_error_code (edi_parser_t * this)
{
  return this ? (int) this->error : 0;
}

/** \brief Returns the interchange type of the stream. */
edi_interchange_type_t
edi_parser_interchange_type (edi_parser_t * this)
{
  return this ? this->interchange_type : EDI_UNKNOWN;
}

/** \brief Obsolete. */
edi_parameters_t *
edi_parser_info (edi_parser_t * this)
{
  /*return (this && this->parser_info) ? this->parser_info (this) : NULL; */
  return NULL;
}

/** \brief Returns a pointer to the service directory */
edi_directory_t *
edi_parser_service (edi_parser_t * this)
{
  return this->service;
}

/** \brief Returns a pointer to the message directory */
edi_directory_t *
edi_parser_message (edi_parser_t * this)
{
  return this->message;
}

edi_pragma_t
edi_set_pragma_t (edi_parser_t * p, edi_pragma_t pragma)
{
  edi_pragma_t old = p->pragma;
  p->pragma = pragma;
  return (old);
}














/**********************************************************************
 * Deal with potential errors raised by various subsytems
 **********************************************************************/

/** \brief Error handling is not currently well-defined */
edi_error_t
edi_parser_raise_error (edi_parser_t * this, edi_error_t error)
{
  int warning = 0;

  if (error == EDI_ENONE)
    return error;

  switch (error)
    {
    case EDI_ECHARSET:
      warning = this->pragma & EDI_PCHARSET;
      break;

    case EDI_ESEGMENT:
      warning = this->pragma & EDI_PSEGMENT;
      break;

    default:
      warning = 0;
    }

  if (warning && this->warning_handler)
    this->warning_handler (this->user_data, error);

  if (!warning && this->error_handler)
    this->error_handler (this->user_data, error);

  if (!warning)
    {
      /*printf("*** %d\n", error); */
      this->error = error;
      this->tokeniser.error = error;
    }


  return warning ? EDI_ENONE : error;
}

















/**********************************************************************
 * Wrapper functions for handing events off to the client application
 **********************************************************************/

/** \brief Notifies the client of a complete segment */
void edi_parser_handle_segment
  (edi_parser_t * this, edi_parameters_t * p, edi_directory_t * d)
{
  /* HACK - drain tokens in the queue when segment is handled */
  /* (eventually "segments" won't be handled, but a list of tokens will) */

  edi_token_t *token;
  while ((token = edi_queue_dequeue (&(this->token_queue))))
    {
      edi_parser_handle_token (this, token);
      free (token);
    }

  if (this->segment_handler)
    this->segment_handler (this->user_data, p, this->segment, d);
}

/** \brief Requests the client for a directory (to parse transaction) */
edi_directory_t *
edi_parser_handle_directory (edi_parser_t * this, edi_parameters_t * p)
{
  edi_parameters_set_one (p, Event, "directory");
  return (this->directory_handler ?
	  this->directory_handler (this->user_data, p) : NULL);
}

/** \brief Notifies the client of the start of a structural event */
void edi_parser_handle_start
  (edi_parser_t * this, edi_event_t event, edi_parameters_t * p)
{
  if (this->start_handler)
    this->start_handler (this->user_data, event, p);
}

/** \brief Notifies the client of the end of a structural event */
void edi_parser_handle_end
  (edi_parser_t * this, edi_event_t event, edi_parameters_t * p)
{
  if (this->end_handler)
    this->end_handler (this->user_data, event, NULL);
}

/** \brief Notifies the client of a (possibly partial) token */
void
edi_parser_handle_token (edi_parser_t * this, edi_token_t * token)
{
  if (this->token_handler)
    this->token_handler (this->user_data, token);
}


























/**********************************************************************
 * Functions for building up the segment data structure
 **********************************************************************/

void
edi_parser_clear_buffer (edi_parser_t * this)
{
  edi_buffer_clear (&(this->parse_buffer));
}

void
edi_parser_add_to_buffer (edi_parser_t * this, char c)
{
  edi_buffer_append (&(this->parse_buffer), &c, 1);
}

void
edi_parser_end_tag (edi_parser_t * this)
{
  edi_segment_set_code (this->segment,
			edi_parser_get_buffer_data (this),
			edi_parser_get_buffer_size (this));
  edi_parser_clear_buffer (this);
  this->de = 0;
  this->cde = 0;
}

void
edi_parser_end_element (edi_parser_t * this)
{
  edi_segment_set_element (this->segment, this->de++, this->cde,
			   edi_parser_get_buffer_data (this),
			   edi_parser_get_buffer_size (this));
  edi_parser_clear_buffer (this);
  this->cde = 0;
}

void
edi_parser_end_subelement (edi_parser_t * this)
{
  edi_segment_set_element (this->segment, this->de, this->cde++,
			   edi_parser_get_buffer_data (this),
			   edi_parser_get_buffer_size (this));
  edi_parser_clear_buffer (this);
}

void
edi_parser_new_segment (edi_parser_t * this)
{
  edi_parser_clear_buffer (this);
  edi_segment_clear (this->segment);
  this->segment_count++;
  this->cde = 0;
  this->de = 0;
}

int
edi_parser_get_buffer_size (edi_parser_t * this)
{
  return edi_buffer_size (&(this->parse_buffer));
}

char *
edi_parser_get_buffer_data (edi_parser_t * this)
{
  return edi_buffer_data (&(this->parse_buffer));
}




























/**********************************************************************
 * Functions for building an internal progress stack
 **********************************************************************/

int
edi_parser_push_segment (edi_parser_t * this, edi_segment_t * segment)
{
  edi_segment_t *copy;

  if ((copy = edi_segment_dup (segment)))
    edi_stack_push (&(this->stack), copy);

  return copy ? 1 : 0;
}

void
edi_parser_pop_segment (edi_parser_t * this)
{
  edi_segment_free (edi_stack_pop (&(this->stack)));
}

edi_segment_t *
edi_parser_peek_segment (edi_parser_t * this)
{
  return (edi_segment_t *) edi_stack_peek (&this->stack);
}


























/**********************************************************************
 * Functions for the client application to set its handler functions
 **********************************************************************/

edi_structure_handler_t
edi_parser_set_start_handler (edi_parser_t * this, edi_structure_handler_t h)
{
  edi_structure_handler_t old = this->start_handler;
  this->start_handler = h;
  return old;
}

edi_structure_handler_t
edi_parser_set_end_handler (edi_parser_t * this, edi_structure_handler_t h)
{
  edi_structure_handler_t old = this->end_handler;
  this->end_handler = h;
  return old;
}

edi_error_handler_t
edi_parser_set_error_handler (edi_parser_t * this, edi_error_handler_t h)
{
  edi_error_handler_t old = this->error_handler;
  this->error_handler = h;
  return old;
}

edi_error_handler_t
edi_parser_set_warning_handler (edi_parser_t * this, edi_error_handler_t h)
{
  edi_error_handler_t old = this->warning_handler;
  this->warning_handler = h;
  return old;
}

edi_token_handler_t
edi_parser_set_token_handler (edi_parser_t * this, edi_token_handler_t h)
{
  edi_token_handler_t old = this->token_handler;
  this->token_handler = h;
  return old;
}

edi_directory_handler_t
edi_parser_set_directory_handler (edi_parser_t * this,
				  edi_directory_handler_t d)
{
  edi_directory_handler_t old = this->directory_handler;
  this->directory_handler = d;
  return old;
}

edi_segment_handler_t
edi_parser_set_segment_handler (edi_parser_t * this, edi_segment_handler_t h)
{
  edi_segment_handler_t old = this->segment_handler;
  this->segment_handler = h;
  return old;
}

void *
edi_parser_set_user_data (edi_parser_t * this, void *user_data)
{
  void *old = this->user_data;
  this->user_data = user_data;
  return old;
}



























/**********************************************************************
 * Handlers for events caused by lower level parser
 **********************************************************************/


/**
   \brief Called when the tokeniser completes a token
   \param v Pointer to the edi_parser_t structure
   \param token Pointer to the completed token

   When a token is completed by the low-level tokeniser this function
   is called with a pointer to the completed token. A sequence of
   tokens is used to build an edi_segment_t structure and tokens are
   also saved in a queue for processing after any events caused by
   (but logically preceeding) the current segment have been
   dispatched.
*/

void
edi_parser_token_handler (void *v, edi_token_t * token)
{
  edi_parser_t *this = (edi_parser_t *) v;
  edi_token_t *copy_of_token;

  if ((copy_of_token = malloc (sizeof (edi_token_t))))
    {
      *copy_of_token = *token;
      edi_queue_queue (&(this->token_queue), copy_of_token);
    }

  /* fprintf(stderr, "+++ %d[%.*s]\n", token->type,
     (int) token->csize, token->cdata); */

  switch (token->type)
    {
    case EDI_ST:
      edi_parser_end_element (this);
      if (this->sgmnt_handler)
	this->sgmnt_handler (this);
      edi_parser_new_segment (this);
      break;

    case EDI_SS:
      edi_parser_end_subelement (this);
      break;

    case EDI_ES:
      edi_parser_end_element (this);
      break;

    case EDI_TS:
      edi_parser_end_tag (this);
      break;

    case EDI_TG:
    case EDI_TX:
      edi_buffer_append (&(this->parse_buffer), token->cdata, token->csize);
      break;

    default:
      /* FIXME unknown token type - do nothing */
      break;
    }

  if (token->type == EDI_ST)
    while ((token = edi_queue_dequeue (&(this->token_queue))))
      free (token);

  return;
}



/**
   \brief Called when the tokeniser recognises the interchange type (syntax)
   \param v Pointer to the edi_parser_t structure
   \param type Interchange type of the stream

   When the low-level tokeniser recognises the syntax that is in use
   this function is called and is used to "morph" the parser in to the
   respective syntax parser. This is done by calling the appropriate
   syntax "_init" function which sets various function pointers on the
   edi_parser_s structure.
*/

void edi_parser_itype_handler (void *v, edi_interchange_type_t type)
{
  edi_parser_t *this = (edi_parser_t *) v;

  this->interchange_type = type;

  switch (type)
    {
    case EDI_EDIFACT:
      edi_edifact_init (this);
      break;

    case EDI_UNGTDI:
      edi_ungtdi_init (this);
      break;

    case EDI_X12:
      edi_x12_init (this);
      break;

    case EDI_UNKNOWN:
      return;
    }
}















/** \} */
