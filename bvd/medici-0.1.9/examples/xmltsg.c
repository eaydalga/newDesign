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

/**********************************************************************
 *
 * This file reads a simple XML based format containing transaction set
 * guidelines and creates an edi_giovanni_t structure.
 *
 * If HAVE_UNISTD_H is defined an extra function which reads the file
 * in from a UNIX file descriptor is defined - this is useful for
 * reading from the DATA file handle in Perl.
 *
 **********************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBEXPAT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <expat.h>

#include <medici.h>

#include "../src/adt.h"
#include "../src/segment.h"
#include "../src/drctry.h"
#include "../src/giovanni.h"


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define MY_BUFFSIZE    8192

#define MY_ELEMENT     0
#define MY_COMPOSITE   1
#define MY_SEGMENT     2
#define MY_CODELIST    3
#define MY_TRANSACTION 4
#define MY_VALUE       5
#define MY_ELEMREF     6
#define MY_SEGREF      7
#define MY_LOOP        8
#define MY_COMPONENT   9
#define MY_UNKNOWN     -1


static char *
mystrdup (const char *src)
{
  char *dst;
  unsigned int n;

  if (!src || !src[0])
    return NULL;

  n = strlen (src);

  if ((dst = malloc (n + 1)))
    {
      strncpy (dst, src, n);
      dst[n] = '\0';
    }

  return dst;
}



static void
entity_attrs (edi_gitem_t * entity, const char **attr)
{
  int i;
  const char *key, *value;

  for (i = 0; attr[i]; i += 2)
    {
      key = attr[i];
      value = attr[i + 1];

      if (!value)
	continue;

      if (!strcmp (key, "code"))
	entity->item.code = mystrdup (value);
      else if (!strcmp (key, "name"))
	entity->item.name = mystrdup (value);
      else if (!strcmp (key, "desc"))
	entity->item.desc = mystrdup (value);
      else if (!strcmp (key, "func"))
	entity->item.desc = mystrdup (value);
      else if (!strcmp (key, "note"))
	entity->item.note = mystrdup (value);
      else if (!strcmp (key, "min"))
	entity->item.min = atoi (value);
      else if (!strcmp (key, "max"))
	entity->item.max = atoi (value);
      else if (!strcmp (key, "reps"))
	entity->item.reps = atoi (value);

      else if (!strcmp (key, "reqr") && !strcmp (value, "mandatory"))
	entity->item.reqr = 1;
      else if (!strcmp (key, "reqr") && !strcmp (value, "conditional"))
	entity->item.reqr = 0;

      else if (!strcmp (key, "repr") && !strcmp (value, "alpha"))
	entity->item.repr = EDI_ISO2382A;
      else if (!strcmp (key, "repr") && !strcmp (value, "numeric"))
	entity->item.repr = EDI_ISO2382N;
      else if (!strcmp (key, "repr") && !strcmp (value, "mixed"))
	entity->item.repr = EDI_ISO2382X;
      else if (!strcmp (key, "repr") && !strcmp (value, "1decimal"))
	entity->item.repr = EDI_TRADACOMS1;
      else if (!strcmp (key, "repr") && !strcmp (value, "2decimal"))
	entity->item.repr = EDI_TRADACOMS2;
      else if (!strcmp (key, "repr") && !strcmp (value, "3decimal"))
	entity->item.repr = EDI_TRADACOMS3;
      else if (!strcmp (key, "repr") && !strcmp (value, "4decimal"))
	entity->item.repr = EDI_TRADACOMS4;
      else if (!strcmp (key, "repr") && !strcmp (value, "integer"))
	entity->item.repr = EDI_INTEGER;
      else if (!strcmp (key, "repr") && !strcmp (value, "real"))
	entity->item.repr = EDI_REAL;

      else if (!strcmp (key, "type") && !strcmp (value, "simple"))
	entity->item.type = 0;
      else if (!strcmp (key, "type") && !strcmp (value, "composite"))
	entity->item.type = 1;
    }
}




static int
elementtype (const char *el)
{
  if (!strcmp (el, "segment"))
    return MY_SEGMENT;
  else if (!strcmp (el, "composite"))
    return MY_COMPOSITE;
  else if (!strcmp (el, "element"))
    return MY_ELEMENT;
  else if (!strcmp (el, "codelist"))
    return MY_CODELIST;
  else if (!strcmp (el, "transaction"))
    return MY_TRANSACTION;
  else if (!strcmp (el, "value"))
    return MY_VALUE;
  else if (!strcmp (el, "elemref"))
    return MY_ELEMREF;
  else if (!strcmp (el, "segref"))
    return MY_SEGREF;
  else if (!strcmp (el, "loop"))
    return MY_LOOP;
  else if (!strcmp (el, "component"))
    return MY_COMPONENT;
  else
    return MY_UNKNOWN;
}




static void
start (void *data, const char *el, const char **attr)
{
  int type;
  edi_gitem_t *entity;
  edi_list_t *list = NULL;
  edi_giovanni_t *tsg = (edi_giovanni_t *) data;

  if ((type = elementtype (el)) == MY_UNKNOWN)
    return;

  /* FIXME - handle failures better */
  if (!(entity = malloc (sizeof (edi_gitem_t))))
    {
      tsg->current = NULL;
      return;
    }

  edi_list_unshift (&(tsg->everything), entity);

  memset (entity, 0, sizeof (edi_gitem_t));

  entity_attrs (entity, attr);

  switch (type)
    {
    case MY_SEGMENT:
      list = &(tsg->segments);
      tsg->current = entity;
      break;
    case MY_COMPOSITE:
      list = &(tsg->composites);
      tsg->current = entity;
      break;
    case MY_ELEMENT:
      list = &(tsg->elements);
      tsg->current = entity;
      break;
    case MY_TRANSACTION:
      list = &(tsg->transactions);
      edi_stack_push (&(tsg->stack), entity);
      tsg->current = entity;
      break;
    case MY_LOOP:
      entity->item.type = 1;
      list = tsg->current ? &(tsg->current->list) : NULL;
      edi_stack_push (&(tsg->stack), entity);
      tsg->current = entity;
      break;
    case MY_CODELIST:
      /* associate codelist values with corresponding element */
      tsg->current = (edi_gitem_t *)
	edi_giovanni_find_element ((edi_directory_t *) tsg,
				   entity->item.code);
      break;
    case MY_COMPONENT:
    case MY_ELEMREF:
    case MY_VALUE:
    case MY_SEGREF:
      list = tsg->current ? &(tsg->current->list) : NULL;
      break;
    default:
      list = NULL;
      break;
    }

  if (list)
    edi_list_push_key (list, entity->item.code, entity);
}




static void
end (void *data, const char *el)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) data;

  switch (elementtype (el))
    {
    case MY_TRANSACTION:
    case MY_LOOP:
      edi_stack_pop (&(tsg->stack));
      tsg->current = edi_stack_peek (&(tsg->stack));
      break;
    default:
      break;
    }
}






edi_directory_t *
read_xmltsg_buffer (char *buff, unsigned int len)
{
  /* read_xmltsg is based on the code in the examples directory in expat */
  XML_Parser p = NULL;
  edi_giovanni_t *tsg = NULL;

  if (!(tsg = (edi_giovanni_t *) edi_giovanni_create ()))
    {
      perror ("Couldn't create TSG");
      goto cleanup;
    }

  if (!(p = XML_ParserCreate (NULL)))
    {
      perror ("Couldn't create XML parser");
      goto cleanup;
    }

  XML_SetUserData (p, tsg);
  XML_SetElementHandler (p, start, end);

  if (XML_Parse (p, buff, len, 1) == 0)
    {
      fprintf (stderr, "Parse error in XML file at line %d: %s\n",
	       XML_GetCurrentLineNumber (p),
	       XML_ErrorString (XML_GetErrorCode (p)));
      goto cleanup;
    }

  /* clear down stack in case xml reading routine didn't do it properly */
  edi_stack_clear (&(tsg->stack), NULL);

  XML_ParserFree (p);

  return (edi_directory_t *) tsg;

cleanup:

  if (p)
    XML_ParserFree (p);

  if (tsg)
    edi_directory_free ((edi_directory_t *) tsg);

  return NULL;
}




edi_directory_t *
read_xmltsg_stream (FILE * stream)
{
  /* read_xmltsg is based on the code in the examples directory in expat */
  char buff[MY_BUFFSIZE];
  XML_Parser p = NULL;
  edi_giovanni_t *tsg = NULL;

  if (!(tsg = (edi_giovanni_t *) edi_giovanni_create ()))
    {
      perror ("Couldn't create TSG");
      goto cleanup;
    }

  if (!(p = XML_ParserCreate (NULL)))
    {
      perror ("Couldn't create XML parser");
      goto cleanup;
    }

  XML_SetUserData (p, tsg);
  XML_SetElementHandler (p, start, end);

  for (;;)
    {
      int done;
      int len;

      len = fread (buff, 1, MY_BUFFSIZE, stream);
      if (ferror (stream))
	{
	  perror ("Error reading XML file");
	  goto cleanup;
	}
      done = feof (stream);

      if (XML_Parse (p, buff, len, done) == 0)
	{
	  fprintf (stderr, "Parse error in XML file at line %d: %s\n",
		   XML_GetCurrentLineNumber (p),
		   XML_ErrorString (XML_GetErrorCode (p)));
	  goto cleanup;
	}

      if (done)
	break;
    }

  /* clear down stack in case xml reading routine didn't do it properly */
  edi_stack_clear (&(tsg->stack), NULL);

  XML_ParserFree (p);

  return (edi_directory_t *) tsg;

cleanup:

  if (p)
    XML_ParserFree (p);

  if (tsg)
    edi_directory_free ((edi_directory_t *) tsg);

  return NULL;
}

edi_directory_t *
read_xmltsg (char *file)
{
  /* read_xmltsg is based on the code in the examples directory in expat */
  edi_directory_t *tsg;
  FILE *stream;

  if (!(stream = fopen (file, "r")))
    {
      perror ("Couldn't open XML file");
      return NULL;
    }

  tsg = read_xmltsg_stream (stream);

  fclose (stream);

  return tsg;
}








#ifdef HAVE_UNISTD_H
edi_directory_t *
read_xmltsg_fd (int fd)
{
  /* read_xmltsg is based on the code in the examples directory in expat */
  char buff[MY_BUFFSIZE];
  XML_Parser p = NULL;
  edi_giovanni_t *tsg = NULL;

  if (!(tsg = (edi_giovanni_t *) edi_giovanni_create ()))
    {
      perror ("Couldn't create TSG");
      goto cleanup;
    }

  if (!(p = XML_ParserCreate (NULL)))
    {
      perror ("Couldn't create XML parser");
      goto cleanup;
    }

  XML_SetUserData (p, tsg);
  XML_SetElementHandler (p, start, end);

  for (;;)
    {
      int done;
      ssize_t len;

      len = read (fd, buff, MY_BUFFSIZE);
      if (len == -1)
	{
	  perror ("Error reading XML file");
	  goto cleanup;
	}

      done = !len;

      if (XML_Parse (p, buff, len, done) == 0)
	{
	  fprintf (stderr, "Parse error in XML file at line %d: %s\n",
		   XML_GetCurrentLineNumber (p),
		   XML_ErrorString (XML_GetErrorCode (p)));
	  goto cleanup;
	}

      if (done)
	break;
    }

  /* clear down stack in case xml reading routine didn't do it properly */
  edi_stack_clear (&(tsg->stack), NULL);

  XML_ParserFree (p);

  return (edi_directory_t *) tsg;

cleanup:

  if (p)
    XML_ParserFree (p);

  if (tsg)
    edi_directory_free ((edi_directory_t *) tsg);

  return NULL;
}

#endif /* HAVE_UNISTD_H */
#endif /* HAVE_LIBEXPAT */
