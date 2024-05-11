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
#include <stdio.h>

#include "internal.h"

void SYNTAX_init (FSAutomaton * fsa);


/*! \file token.c */

/**
   \defgroup edi_token edi_token

   \brief An EDI lexical token

   \{
*/

/**
   \brief Initialises an edi_token_s structure.
   \param this Pointer to the structure to be initialised.
*/
void
edi_token_init (edi_token_t * this)
{
  this->type = EDI_TX;
  this->offset = 0;
  this->csize = 0;
  this->rsize = 0;
  this->cdata[0] = '\0';
  this->rdata[0] = '\0';
  this->first = 1;
  this->last = 0;
}

/** \} */





/**
   \defgroup edi_tokeniser edi_tokeniser

   \brief The lexical analyser component of an EDI stream parser.

   edi_tokeniser breaks a stream of characters down into a stream of
   edi_token_s structs. The edi_tokeniser_s object contains state
   information and callback handlers for passing tokens to the next
   level of parsing.

   edi_tokeniser_t contains a single edi_token_s struct which is
   reused as each token is read. Long data elements will be
   represented as multiple callbacks to the token handler with first
   and last flags set on the edi_token_s as appropiate. This means
   that no memory allocation from the heap is needed at this level of
   the parser.

   \{ */

/**
   \brief Initialises an edi_tokeniser_s structure.
   \param this Pointer to the structure to be initialised.
*/
void edi_tokeniser_init (edi_tokeniser_t * this)
{
  if (!this)
    return;
  memset (this, 0, sizeof (this));	/* mitigate bugs */
  this->offset = 0;
  this->token_process = NULL;
  this->itype_handler = NULL;
  this->token_handler = NULL;
  this->error_handler = NULL;
  edi_token_init (&(this->token));
  edi_advice_init (&(this->advice));
  this->state = 0;
  this->error = EDI_ENONE;
  this->release = 1;
  this->user_data = NULL;
  this->byte_count = 0;
  SYNTAX_init (&(this->fsa));
}


/**
   \brief Causes the current token to be passed to callback handlers.
   \param this Pointer to the edi_tokeniser_s structure.

   Hands off processing of the current token to the registered
   callback handlers and initialises the next token.
*/
void
edi_tokeniser_handle_token (edi_tokeniser_t * this, int last)
{
  this->token.last = last;

  if (this->token_handler)
    this->token_handler (this->user_data, &(this->token));

  /* clear data from the token and update the offset to reflect the */
  /* position in the stream of the following token */

  this->token.offset += this->token.rsize;
  this->token.rsize = this->token.csize = 0;

  if (last)
    this->token.type = EDI_TX;

  /* If we just handled the last token in a series the next one will be */
  /* the first in a new series of tokens. */
  this->token.first = last;
  this->token.last = 0;
}

/**
   \brief Parse a string of characters.
   \param this Pointer to the edi_tokeniser_s structure.
   \param buffer The string of characters to be parsed.
   \param size The size of the buffer (in characters).
   \param done Set to non-zero to indicate that the stream is complete.
   \return The number of characters processed. Note that a return value
   of zero does not indicate an error - the error status of the
   tokeniser should be explicitly checked.

*/
unsigned int edi_tokeniser_parse
  (edi_tokeniser_t * this, char *buffer, unsigned int size, int done)
{
  unsigned int n;
  edi_interchange_type_t itype = EDI_UNKNOWN;

  /* if we already know what syntax is handling this stream we hand
     over parsing of tokens to it */
  if (this->token_process)
    return this->token_process (this, buffer, size);

  /* Try to autodetect the syntax type based on the first few characters */
  for (n = 0; n < size; n++)
    {
      if (this->offset < 3)
	this->autotype[this->offset] = buffer[n];

      if (++(this->offset) == 3)
	{
	  /* add a trailing nul for string comparisons */
	  this->autotype[3] = '\0';

	  /* try to match the stream to a known syntax */
	  if (!strcmp (this->autotype, "UNA") ||
	      !strcmp (this->autotype, "UNB"))
	    itype = EDI_EDIFACT;
	  else if (!strcmp (this->autotype, "ISA"))
	    itype = EDI_X12;
	  else if (!strcmp (this->autotype, "STX"))
	    itype = EDI_UNGTDI;

	  /* inform the application of the syntax */
	  if (this->itype_handler)
	    this->itype_handler (this->user_data, itype);

	  this->token_process = edi_tokeniser_token_process;

	  /* if the syntax has been detected start the hand over to it ... */
	  if (this->token_process)
	    {
	      /* this caracter has not been officially consumed yet - do it */
	      n++;
	      /* feed the auto-detection buffer to the syntax to prime it */
	      this->token_process (this, this->autotype, 3);
	      /* we need to return the amount of characters consumed */
	      /* in the detection portion plus the characters consumed */
	      /* subsequently by the syntax module */
	      return n + this->token_process (this, buffer + n, size - n);
	    }

	  /* if we fall through to here the syntax was not detected */
	  /* seeing as we don't know what syntax this is we give up */
	  /* FIXME - handle this more gracefully */

	  edi_tokeniser_handle_error (this, EDI_ESYNTAX);
	  return n;
	}
    }

  /* report back however many characters we consumed in this invocation */

  /* note: this will only ever happen if less than the number of characters */
  /* required to autodetect syntax have been processed so far which is */
  /* unlikely, but could happen if characters are being passed individually */
  return n;
}


void
edi_tokeniser_handle_error (edi_tokeniser_t * this, edi_error_t error)
{
  this->error = error;
  if (this->error_handler)
    this->error_handler (this->user_data, error);
}







unsigned int edi_tokeniser_token_process
  (edi_tokeniser_t * this, char *string, unsigned int length)
{
  edi_advice_t *advice = &(this->advice);
  char c, a;
  int n, event;

  for (n = 0; n < length && !this->error; n++)
    {
      c = string[n];
      this->byte_count++;

      if (edi_advice_get_ri (advice, &a) && c == a)
	event = SYNTAX_RI;
      else if (edi_advice_get_ss (advice, &a) && c == a)
	event = SYNTAX_SS;
      else if (edi_advice_get_es (advice, &a) && c == a)
	event = SYNTAX_ES;
      else if (edi_advice_get_ts (advice, &a) && c == a)
	event = SYNTAX_TS;
      else if (edi_advice_get_st (advice, &a) && c == a)
	event = SYNTAX_ST;
      else
	{
	  switch (c)
	    {
	    case ASCII_A:
	      event = SYNTAX_A;
	      break;
	    case ASCII_B:
	      event = SYNTAX_B;
	      break;
	    case ASCII_I:
	      event = SYNTAX_I;
	      break;
	    case ASCII_N:
	      event = SYNTAX_N;
	      break;
	    case ASCII_S:
	      event = SYNTAX_S;
	      break;
	    case ASCII_T:
	      event = SYNTAX_T;
	      break;
	    case ASCII_U:
	      event = SYNTAX_U;
	      break;
	    case ASCII_X:
	      event = SYNTAX_X;
	      break;
	    case ASCII_CR:
	      event = SYNTAX_CR;
	      break;
	    case ASCII_LF:
	      event = SYNTAX_LF;
	      break;
	    default:
	      event = SYNTAX_DEFAULT;
	      break;
	    }
	}

      if (FSAProcess (&(this->fsa), this, c, event))
	{
	  fprintf (stderr, "Syntax error!\n");
	  exit (-1);
	}
      /*fprintf(stderr, "%c(%d,%d) ", c, event, this->fsa.state); */
    }

  return n;
}


int
edi_token_append (edi_token_t * t, char c, int d)
{
  t->rdata[t->rsize++] = c;
  t->rdata[t->rsize] = '\0';
  if (d)
    {
      t->cdata[t->csize++] = c;
      t->cdata[t->csize] = '\0';
    }
  return t->rsize == (EDI_TOKEN_MAX - 1);
}


/** \} */
