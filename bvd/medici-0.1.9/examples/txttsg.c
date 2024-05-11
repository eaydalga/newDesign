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

/*
  txttsg.c
  
  Reads in a TAB-separated specification of a directory from a file
  and returns a pointer to a directory object.
  
  FIXME - define spec file
  
  Data types:
  A - Alpha
  N - Numeric
  X - Alphanumeric
  1 - Implied 1 decimal place  ("10000" => 1000.0)
  2 - Implied 2 decimal places ("10000" => 100.00)
  3 - Implied 3 decimal places ("10000" => 10.000)
  4 - Implied 4 decimal places ("10000" => 1.0000)
  
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <medici.h>

#include "../src/adt.h"
#include "../src/segment.h"
#include "../src/drctry.h"
#include "../src/giovanni.h"

#define HEADER  0
#define SECTION 1
#define DETAIL  2

#define SEGMENT   1
#define LOOP      2
#define ELEMENT   3
#define COMPOSITE 4
#define COMPONENT 5
#define SEGLIST   6
#define CODELIST  7

static char *
mystrdup (char *src)
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


edi_data_type_t
datatype (char *string)
{

  switch (string ? *string : 'X')
    {
    case 'A':
      return EDI_ISO2382A;
    case 'N':
      return EDI_ISO2382N;
    case '1':
      return EDI_TRADACOMS1;
    case '2':
      return EDI_TRADACOMS2;
    case '3':
      return EDI_TRADACOMS3;
    case '4':
      return EDI_TRADACOMS4;
    default:
    case 'X':
      return EDI_ISO2382X;
    }
}

int
reqrtype (char *string)
{
  return string ? (string[0] == 'M') : 0;
}

int
typetype (char *string)
{
  return string ? (string[0] == 'C') : 0;
}

int
myatoint (char *string)
{
  return string ? atoi (string) : 0;
}





void *
read_txttsg (char *file)
{
  FILE *stream;
  char buffer[65536];
  char *tokens[16];
  char *token;
  int n, type, section = 0;
  edi_giovanni_t tmptsg;
  edi_giovanni_t *tsg = &tmptsg;
  edi_gitem_t *entity, *tmp;
  edi_list_t *list;
  edi_directory_t *dir;

  if (!(stream = fopen (file, "r")))
    {
      fprintf (stderr, "Couldn't open %s\n", file);
      return NULL;
    }

  if (!(tsg = (edi_giovanni_t *) edi_giovanni_create ()))
    return NULL;

  dir = (edi_directory_t *) tsg;

  while (fgets (buffer, sizeof (buffer), stream))
    {
      if (strlen (buffer))
	buffer[strlen (buffer) - 1] = '\0';
      else
	continue;

      token = buffer;

      switch (buffer[0])
	{
	case '\0':
	case ' ':
	case '#':
	  continue;

	case '\t':
	  type = DETAIL;
	  token++;
	  break;

	case '!':
	  type = HEADER;
	  token++;
	  continue;

	default:
	  type = SECTION;
	  for (n = 0; token[n]; n++)
	    if (token[n] == '\t')
	      token[n] = '\0';
	  section = 0;
	  if (!strcmp (token, "ELEMENT"))
	    section = ELEMENT;
	  if (!strcmp (token, "COMPOSITE"))
	    section = COMPOSITE;
	  if (!strcmp (token, "COMPONENT"))
	    section = COMPONENT;
	  if (!strcmp (token, "SEGMENT"))
	    section = SEGMENT;
	  if (!strcmp (token, "SEGLIST"))
	    section = SEGLIST;
	  if (!strcmp (token, "CODELIST"))
	    section = CODELIST;
	  if (!strcmp (token, "LOOP"))
	    section = LOOP;
	  continue;
	}

      for (n = 0; n < (sizeof (tokens) / sizeof (char *)); n++)
	tokens[n] = NULL;

      tokens[0] = token;

      for (n = 1; *token; token++)
	if (*token == '\t')
	  {
	    *token = '\0';
	    if (*(token + 1))
	      tokens[n] = token + 1;
	    n++;
	  }

      if (!(entity = malloc (sizeof (edi_gitem_t))))
	break;

      memset (entity, 0, sizeof (edi_gitem_t));

      list = NULL;

      switch (type)
	{
	case DETAIL:
	  switch (section)
	    {
	    case ELEMENT:
	      entity->item.code = mystrdup (tokens[0]);
	      entity->item.repr = datatype (tokens[1]);
	      entity->item.min = myatoint (tokens[2]);
	      entity->item.max = myatoint (tokens[3]);
	      entity->item.name = mystrdup (tokens[4]);
	      entity->item.desc = mystrdup (tokens[5]);
	      entity->item.note = mystrdup (tokens[6]);
	      list = &(tsg->elements);
	      break;

	    case COMPOSITE:
	      entity->item.code = mystrdup (tokens[0]);
	      entity->item.name = mystrdup (tokens[1]);
	      entity->item.desc = mystrdup (tokens[2]);
	      entity->item.note = mystrdup (tokens[3]);
	      list = &(tsg->composites);
	      break;

	    case SEGMENT:
	      entity->item.code = mystrdup (tokens[0]);
	      entity->item.name = mystrdup (tokens[1]);
	      entity->item.desc = mystrdup (tokens[2]);
	      entity->item.note = mystrdup (tokens[3]);
	      list = &(tsg->segments);
	      break;

	    case COMPONENT:
	      if (!(tmp = (edi_gitem_t *)
		    edi_giovanni_find_composite (dir, tokens[0])))
		break;
	      entity->item.code = mystrdup (tokens[1]);
	      entity->item.reqr = reqrtype (tokens[2]);
	      list = &(tmp->list);
	      break;

	    case SEGLIST:
	      if (!(tmp = (edi_gitem_t *)
		    edi_giovanni_find_segment (dir, tokens[0])))
		break;
	      entity->item.code = mystrdup (tokens[1]);
	      entity->item.reqr = reqrtype (tokens[2]);
	      entity->item.type = typetype (tokens[3]);
	      list = &(tmp->list);
	      break;

	    case CODELIST:
	      if (!(tmp = (edi_gitem_t *)
		    edi_giovanni_find_element (dir, tokens[0])))
		break;
	      entity->item.code = mystrdup (tokens[1]);
	      entity->item.name = mystrdup (tokens[2]);
	      entity->item.desc = mystrdup (tokens[3]);
	      entity->item.note = mystrdup (tokens[4]);
	      list = &(tmp->list);
	      break;

	    case LOOP:
	      break;
	    }
	}

      if (list)
	{
	  edi_list_push_key (list, entity->item.code, entity);
	  edi_list_unshift (&(tsg->everything), entity);
	}
      else
	{
	  free (entity->item.code);
	  free (entity->item.name);
	  free (entity->item.desc);
	  free (entity->item.note);
	  free (entity);
	}

    }

  return tsg;
}
