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


/** \file giovanni.c

    \brief An example of how to write a Transaction Set Guidelines module.
    
    This is an example implementation of a Transaction Set Guidelines
    module.  It is included in the main MEDICI library for convenience
    and is not required by any core modules.  The example code
    supplied with MEDICI uses it to implement TAB separated and XML
    based TSG configuration files.

    The data is stored as linear lists so searching is relatively
    slow, but then, this is just an example. To create your own more
    efficient implementation simply create functions which can be
    referenced by the function pointers in the edi_directory_s
    structure and declare a "constructor" function which allocates and
    initialises the edi_directory_s and other data
    (edi_giovanni_create in this instance).

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "giovanni.h"

#define cmpfn ((int (*)(void *, void *)) strcmp)


/**
   \defgroup edi_giovanni edi_giovanni
   \{
*/



edi_item_t *
edi_giovanni_find_element (edi_directory_t * directory, char *code)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  return tsg && code ? edi_list_find (&(tsg->elements), code, cmpfn) : NULL;
}

edi_item_t *
edi_giovanni_find_composite (edi_directory_t * directory, char *code)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  return tsg && code ? edi_list_find (&(tsg->composites), code, cmpfn) : NULL;
}

edi_item_t *
edi_giovanni_find_segment (edi_directory_t * directory, char *code)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  return tsg && code ? edi_list_find (&(tsg->segments), code, cmpfn) : NULL;
}

edi_item_t *edi_giovanni_find_codelist
  (edi_directory_t * directory, char *element, char *code)
{
  edi_gitem_t *entity;

  if (!
      (entity =
       (edi_gitem_t *) edi_giovanni_find_element (directory, element)))
    return NULL;

  if (!(entity = edi_list_find (&(entity->list), code, cmpfn)))
    return NULL;

  return (edi_item_t *) entity;
}








static unsigned int
segment_size (edi_directory_t * directory, char *ref)
{
  edi_gitem_t *entity;
  entity = (edi_gitem_t *) edi_giovanni_find_segment (directory, ref);
  return entity ? edi_list_length (&(entity->list)) : 0;
}

static edi_item_t segment_item
  (edi_directory_t * directory, char *code, unsigned int i)
{
  edi_gitem_t *entity;
  edi_item_t item = EDI_NULL_ITEM, *pitem;
  edi_node_t *node;

  if (!(entity = (edi_gitem_t *) edi_giovanni_find_segment (directory, code)))
    return item;

  for (node = edi_list_head (&(entity->list)); node; node = node->next)
    if (!i-- && (entity = node->data))
      {
	if (entity->item.type)
	  pitem = edi_giovanni_find_composite (directory, entity->item.code);
	else
	  pitem = edi_giovanni_find_element (directory, entity->item.code);

	if (pitem)
	  item = *pitem;

	item.type = entity->item.type;
	item.repr = entity->item.repr;
      }

  return item;
}


static unsigned int
composite_size (edi_directory_t * directory, char *ref)
{
  edi_gitem_t *entity;
  entity = (edi_gitem_t *) edi_giovanni_find_composite (directory, ref);
  return entity ? edi_list_length (&(entity->list)) : 0;
}

static edi_item_t composite_item
  (edi_directory_t * directory, char *code, unsigned int i)
{
  edi_gitem_t *entity;
  edi_item_t item = EDI_NULL_ITEM, *pitem;
  edi_node_t *node;

  if (!code)
    return item;

  if (!
      (entity =
       (edi_gitem_t *) edi_giovanni_find_composite (directory, code)))
    return item;

  for (node = edi_list_head (&(entity->list)); node; node = node->next)
    if (!i-- && (entity = node->data))
      {
	if ((pitem =
	     edi_giovanni_find_element (directory, entity->item.code)))
	  item = *pitem;

	item.reqr = entity->item.reqr;
      }

  return item;
}


































static char *
codelist_name (edi_directory_t * directory, char *ref1, char *ref2)
{
  edi_item_t *item = edi_giovanni_find_codelist (directory, ref1, ref2);
  return item ? item->name : NULL;
}

static char *
codelist_desc (edi_directory_t * directory, char *ref1, char *ref2)
{
  edi_item_t *item = edi_giovanni_find_codelist (directory, ref1, ref2);
  return item ? item->desc : NULL;
}

static char *
codelist_note (edi_directory_t * directory, char *ref1, char *ref2)
{
  edi_item_t *item = edi_giovanni_find_codelist (directory, ref1, ref2);
  return item ? item->note : NULL;
}


static char *
element_name (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_element (directory, ref);
  return item ? item->name : NULL;
}

static char *
element_desc (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_element (directory, ref);
  return item ? item->desc : NULL;
}

static char *
element_note (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_element (directory, ref);
  return item ? item->note : NULL;
}


static char *
composite_name (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_composite (directory, ref);
  return item ? item->name : NULL;
}

static char *
composite_desc (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_composite (directory, ref);
  return item ? item->desc : NULL;
}

static char *
composite_note (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_composite (directory, ref);
  return item ? item->note : NULL;
}


static char *
segment_name (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_segment (directory, ref);
  return item ? item->name : NULL;
}

static char *
segment_desc (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_segment (directory, ref);
  return item ? item->desc : NULL;
}

static char *
segment_note (edi_directory_t * directory, char *ref)
{
  edi_item_t *item = edi_giovanni_find_segment (directory, ref);
  return item ? item->note : NULL;
}

static edi_item_t
element_repr (edi_directory_t * directory, char *code)
{
  edi_item_t *item, null = EDI_NULL_ITEM;
  return (item = edi_giovanni_find_element (directory, code)) ? *item : null;
}




























































static edi_node_t *
first_node (edi_gitem_t * entity)
{
  edi_list_t *list;
  list = &(entity->list);
  return list->first;
}




static edi_error_t
start_transaction (edi_directory_t * directory, char *transaction)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  edi_gitem_t *entity;
  edi_giterator_t *iterator;

  if (!tsg)
    return EDI_EBADTSG;

  edi_stack_init (&(tsg->stack));

  if (!(entity = edi_list_find (&(tsg->transactions), transaction, cmpfn)))
    return EDI_EBADTSG;

  if (!(iterator = malloc (sizeof (edi_giterator_t))))
    return EDI_EBADTSG;

  iterator->node = first_node (entity);
  iterator->reps = 0;

  edi_stack_push (&(tsg->stack), iterator);

  return EDI_ENONE;
}


static edi_error_t
end_transaction (edi_directory_t * directory)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  edi_giterator_t *iterator;
  edi_stack_t *stack;

  if (!tsg)
    return EDI_EBADTSG;

  /* shorthand */
  stack = &(tsg->stack);

  /* clear down stack */
  while (edi_stack_size (stack))
    {
      iterator = edi_stack_pop (stack);
      free (iterator);
    }

  return EDI_ENONE;
}





static void
giovanni_free (edi_directory_t * directory)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  edi_gitem_t *item;

  if (!tsg)
    return;

  /* clear down the finder lists (but NOT the data that the nodes point to) */

  edi_list_drain (&(tsg->elements), NULL);
  edi_list_drain (&(tsg->segments), NULL);
  edi_list_drain (&(tsg->composites), NULL);
  edi_list_drain (&(tsg->transactions), NULL);

  /* Now remove each item in turn */

  while ((item = edi_list_shift (&(tsg->everything))))
    {
      /*fprintf(stderr, "%6ld\n", edi_list_length(&(tsg->everything))); */

      /* clear the item's list (but NOT the data that the nodes point to) */
      edi_list_drain (&(item->list), NULL);

      /* free the character strings associated with this item */
      free (item->item.code);
      free (item->item.name);
      free (item->item.desc);
      free (item->item.note);

      /* free the memory for the item structure itself */
      free (item);
    }

  /* finally, free the memory for the directory structure */
  free (tsg);
}














































typedef void (*edi_callback_t) (void *, edi_parameters_t *,
				edi_directory_t *);
typedef void (*edi_callbacks_t) (void *, edi_event_t, edi_parameters_t *);


/* This routine is probably quite buggy */

static edi_error_t iterate_transaction
  (edi_directory_t * directory, char *code, edi_parameters_t * parameters,
   void *userdata,
   edi_callbacks_t start, edi_callbacks_t end, edi_callback_t segment)
{
  edi_giovanni_t *tsg = (edi_giovanni_t *) directory;
  edi_giterator_t *iterator, *new_iterator;
  edi_gitem_t *entity, *new_entity;

  edi_parameters_t empty;
  edi_stack_t *stack;
  edi_error_t error = EDI_EBADTSG;
  edi_node_t *node;

  if (!tsg)
    return EDI_EBADTSG;

  edi_parameters_set (&empty, LastParameter);

  /* shorthand */
  stack = &(tsg->stack);

  while ((iterator = edi_stack_peek (stack)))
    {
      /* have we run off the end of the current loop ? */
      if (!iterator->node)
	{
	  /* send end loop */
	  parameters->event = EDI_LOOP;

	  if (end)
	    end (userdata, EDI_LOOP, &empty);

	  edi_stack_pop (stack);

	  if ((iterator = edi_stack_peek (stack)))
	    iterator->reps++;

	  continue;
	}

      if (!(entity = iterator->node->data))
	return EDI_EBADTSG;

      /* if we have exceeded the reps for this node move on to the next one */

      if (iterator->reps >= entity->item.reps)
	{
	  iterator->reps = 0;
	  iterator->node = iterator->node->next;
	  continue;
	}

      /* is this node marking the start of a loop ? */

      if (entity->item.type)
	{
	  /* Sanity checks */

	  if (!(node = first_node (entity)) || !node->data)
	    {
	      error = EDI_EBADTSG;
	      goto reject_segment;
	    }

	  /* The first node in a loop MUST be a segment */

	  if (((edi_gitem_t *) node->data)->item.type)
	    {
	      error = EDI_EBADTSG;
	      goto reject_segment;
	    }

	  edi_parameters_set_one (parameters, Code, entity->item.code);
	  edi_parameters_set_one (parameters, Name, entity->item.name);
	  edi_parameters_set_one (parameters, Description, entity->item.desc);
	  edi_parameters_set_one (parameters, Note, entity->item.note);

	  /* Try to create a new iterator, which will be put on the stack */

	  if (!(new_iterator = malloc (sizeof (edi_gitem_t))))
	    {
	      error = EDI_ENOMEM;
	      goto reject_segment;
	    }

	  /* Sanity checks */

	  if (!(node = first_node (entity)) || !node->data)
	    {
	      error = EDI_EBADTSG;
	      goto reject_segment;
	    }

	  new_iterator->node = node;
	  new_iterator->reps = 0;
	  new_entity = node->data;

	  /* Does segment match first node in loop? */

	  if (!strcmp (new_entity->item.code, code))
	    {
	      /* Yes, push this iterator onto the stack and accept segment */
	      iterator = new_iterator;
	      entity = new_entity;
	      iterator->reps++;
	      edi_stack_push (stack, iterator);
	      parameters->event = EDI_LOOP;
	      if (start)
		start (userdata, EDI_LOOP, parameters);
	      goto accept_segment;
	    }

	  /* No, discard the new iterator */
	  free (new_iterator);

	  /* If the loop was mandatory and we have not already matched
	     minimum count, then we have an error */

	  if (entity->item.reqr && !iterator->reps)
	    {
	      error = EDI_EREQUIRED;
	      goto reject_segment;
	    }

	  /* Loop wasn't mandatory. Skip it and onto the next loop/segment */
	  iterator->reps = 0;
	  iterator->node = iterator->node->next;
	  continue;
	}

      /* Item was not a loop, so it must must be a segment. Does it match? */

      if (!(strcmp (entity->item.code, code)))
	{
	  /* Yes, accept the segment */
	  goto accept_segment;
	}

      /* No. If the segment was mandatory and we have not already matched
         minimum count, then we have an error */

      if (entity->item.reqr && !iterator->reps)
	{
	  error = EDI_EREQUIRED;
	  goto reject_segment;
	}

      /* OK, it wasn't mandatory, so just update the iterator to point */
      /* to the next loop/segment and start the process again */

      iterator->reps = 0;
      iterator->node = iterator->node->next;
    }

  /* We have fallen off the bottom of the stack - this is an error */
  error = EDI_ECORRUPT;

reject_segment:
  /*printf("--- %s\n", code); */
  if (segment)
    segment (userdata, parameters, directory);
  return error;

accept_segment:
  /*printf("+++ %s\n", code); */
  if (segment)
    segment (userdata, parameters, directory);
  iterator->reps++;
  return EDI_ENONE;
}




static edi_error_t iterate_transaction_wrapper
  (edi_directory_t * directory,
   edi_segment_t * segment,
   edi_parameters_t * parameters, edi_parser_t * parser, edi_directory_t * d)
{
  return iterate_transaction (directory,
			      edi_segment_get_code (segment),
			      parameters,
			      parser,
			      (edi_callbacks_t) edi_parser_handle_start,
			      (edi_callbacks_t) edi_parser_handle_end,
			      (edi_callback_t) edi_parser_handle_segment);
}




edi_directory_t *
edi_giovanni_create (void)
{
  edi_giovanni_t *tsg;
  edi_directory_t *directory;

  if (!(tsg = malloc (sizeof (edi_giovanni_t))))
    return NULL;

  /* quick and dirty way to initialise all the data structures */
  memset (tsg, 0, sizeof (edi_giovanni_t));

  directory = (edi_directory_t *) tsg;

  directory->user_data = tsg;

  /* set all the function pointers in the edi_directory_t struct to
     point to the functions defined in this file. this allows the user
     to call (for example): 

     edi_directory_element_name(dptr, "1001");

     without having to be aware of the underlying implementation.
   */

  directory->element_name = element_name;
  directory->element_desc = element_desc;
  directory->element_note = element_note;
  directory->element_repr = element_repr;

  directory->segment_name = segment_name;
  directory->segment_desc = segment_desc;
  directory->segment_note = segment_note;
  directory->segment_size = segment_size;
  directory->segment_item = segment_item;

  directory->composite_name = composite_name;
  directory->composite_desc = composite_desc;
  directory->composite_note = composite_note;
  directory->composite_size = composite_size;
  directory->composite_item = composite_item;

  directory->codelist_name = codelist_name;
  directory->codelist_desc = codelist_desc;
  directory->codelist_note = codelist_note;

  directory->start = start_transaction;
  directory->parse = iterate_transaction_wrapper;
  directory->end = end_transaction;

  directory->free = giovanni_free;

  return directory;
}





/** \} */
