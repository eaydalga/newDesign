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

#ifndef TOKEN_H
#define TOKEN_H

#define EDI_TOKEN_MAX 64
#define EDI_TOKEN_APPEND_MACRO (t,c,d) \
         ((t->rdata[t->rsize++]=c), \
	  (t->rdata[t->rsize]='\0'), \
          (d && (t->cdata[t->csize++]=c)), \
          (d && (t->cdata[t->csize]='\0')), \
          (t->rsize==(EDI_TOKEN_MAX-1)))

#define EDI_TOKEN_APPEND edi_token_append

typedef struct edi_token_s edi_token_t;
typedef struct edi_tokeniser_s edi_tokeniser_t;

typedef unsigned int (*edi_token_process_t) (void *, char *, unsigned int);
typedef void (*edi_token_handler_t) (void *, edi_token_t *);
typedef void (*edi_error_handler_t) (void *, edi_error_t);
typedef void (*edi_itype_handler_t) (void *, edi_interchange_type_t);


/**
   \brief Type of a parsed EDI token
   
   Details the semiotic class of a group of characters within an EDI
   stream.  Basically this defines a token as being a data separator
   character or a string such as an element or tag code.
   
 */
enum edi_tokentype_s
{
  EDI_TX,			/* Text token */
  EDI_SA,			/* Service Advice token (EDIFACT) */
  EDI_TS,			/* Tag separator character */
  EDI_ES,			/* Element separator character */
  EDI_SS,			/* Subelement separator character */
  EDI_ST,			/* Segment Terminator character */
  EDI_TG			/* Tag token */
};
typedef enum edi_tokentype_s edi_tokentype_t;

/**
   \brief A parsed EDI syntax token

   Contains the raw and cooked data from an EDI stream along with
   context information about the position in the stream and relation
   to neighbouring tokens.
 */
struct edi_token_s
{
  /** \brief Type of the EDI token */
  edi_tokentype_t type;

  /** \brief Position of the start of this token in the stream (first is 0) */
  unsigned long offset;

  /** \brief Number of characters in the "cooked" data buffer */
  unsigned int csize;

  /** \brief Number of characters in the "raw" data buffer */
  unsigned int rsize;

  /** \brief The "cooked" data buffer */
  char cdata[EDI_TOKEN_MAX];

  /** \brief The "raw" data buffer */
  char rdata[EDI_TOKEN_MAX];

  /** \brief Flag indicating the first token in a group */
  char first;

  /** \brief Flag indicating the last token in a group */
  char last;
};

/**
   \brief Lexical analyser for an EDI stream

   Keeps state and callback information for the lowest level of an EDI
   stream parse. All data required is contained within this struct so
   that no use of the heap is needed.
*/
struct edi_tokeniser_s
{
  /** \brief Working space for the current under-construction token */
  edi_token_t token;
  /** \brief Describes the stream discipline */
  edi_advice_t advice;
  /** \brief Syntax-specific handler for lexical analysis of the stream */
  edi_token_process_t token_process;
  /** \brief Handler for recognition of interchange type */
  edi_itype_handler_t itype_handler;
  /** \brief Handler for further analysis of each complete token */
  edi_token_handler_t token_handler;
  /** \brief Handler for errors found in stream */
  edi_error_handler_t error_handler;
  /** \brief State indicator for syntax modules */
  int state;
  edi_error_t error;
  /** \brief State flag to indicate that a character is to be
      interpreted literally */
  char release;
  /** \brief Space to buffer the first few characters of an unknown
      stream type. */
  char autotype[4];
  /** \brief State indicator for the autodetection of unknown stream types */
  unsigned long offset;
  /** \brief Data pointer to be passed to handlers */
  void *user_data;
  /** \brief Finite State Automata for tokenisation */
  FSAutomaton fsa;
  /** \brief Bytes processed so far */
  unsigned long byte_count;
};

/* token.c */
void edi_token_init (edi_token_t *);
void edi_tokeniser_init (edi_tokeniser_t *);
void edi_tokeniser_handle_token (edi_tokeniser_t *, int);
unsigned int edi_tokeniser_parse (edi_tokeniser_t *, char *, unsigned int,
				  int);
void edi_tokeniser_handle_error (edi_tokeniser_t *, edi_error_t);
unsigned int edi_tokeniser_token_process (edi_tokeniser_t *, char *,
					  unsigned int);
int edi_token_append (edi_token_t *, char, int);

#endif /*TOKEN_H */
