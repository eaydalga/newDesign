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
#include <string.h>

#include "adt.h"

static void
edi_node_init (edi_node_t * this, void *key, void *data)
{
  this->key = key;
  this->data = data;
  this->prev = NULL;
  this->next = NULL;
  this->used = 1;
}

static edi_node_t *
edi_node_create (void *key, void *data)
{
  edi_node_t *node;

  if ((node = malloc (sizeof (edi_node_t))))
    edi_node_init (node, key, data);

  return node;
}




/**********************************************************************
 * List
 * Perl style - use a list as a queue or a stack
 **********************************************************************/

void
edi_list_init (edi_list_t * this)
{
  this->length = 0;
  this->first = NULL;
  this->last = NULL;
}

int
edi_list_push (edi_list_t * this, void *data)
{
  return edi_list_push_key (this, NULL, data);
}


int
edi_list_push_key (edi_list_t * this, void *key, void *data)
{
  edi_node_t *node;

  if (!(node = edi_node_create (key, data)))
    return 0;

  node->prev = this->last;
  this->last = node;

  if (node->prev)
    node->prev->next = node;
  else
    this->first = node;

  this->length++;

  return 1;
}


void *
edi_list_pop (edi_list_t * this)
{
  edi_node_t *node;
  void *data;

  if (!(node = this->last))
    return NULL;

  this->last = node->prev;

  if (this->last)
    this->last->next = NULL;
  else
    this->first = NULL;

  this->length--;

  if ((!this->first || !this->last) && this->length)
    fprintf (stderr, "edi_list_pop: no first/last but length not 0!\n");

  data = node->data;
  free (node);
  return data;
}


void *
edi_list_peek (edi_list_t * this)
{
  return this->last ? this->last->data : NULL;
}


void *
edi_list_shift (edi_list_t * this)
{
  edi_node_t *node;
  void *data;

  if (!(node = this->first))
    return NULL;

  this->first = node->next;

  if (this->first)
    this->first->prev = NULL;
  else
    this->last = NULL;

  this->length--;


  if ((!this->first || !this->last) && this->length)
    fprintf (stderr, "edi_list_shift: no first/last but length not 0!\n");

  data = node->data;
  free (node);
  return data;
}



int
edi_list_unshift (edi_list_t * this, void *data)
{
  return edi_list_unshift_key (this, NULL, data);
}


int
edi_list_unshift_key (edi_list_t * this, void *key, void *data)
{
  edi_node_t *node;

  if (!(node = edi_node_create (key, data)))
    return 0;

  node->next = this->first;
  this->first = node;

  if (node->next)
    node->next->prev = node;
  else
    this->last = node;

  if ((!this->first || !this->last) && this->length)
    fprintf (stderr, "edi_list_unshift: no first/last but length not 0!\n");

  this->length++;
  return 1;
}


void *
edi_list_head (edi_list_t * this)
{
  return this->first;
}

void
edi_list_drain (edi_list_t * this, edi_free_t freefn)
{
  void *data;

  while (this->length)
    if ((data = edi_list_shift (this)) && freefn)
      freefn (data);
}


void
edi_list_clear (edi_list_t * this, edi_free_t freefn)
{
  void *data;

  while (this->length)
    if ((data = edi_list_pop (this)) && freefn)
      freefn (data);
}

unsigned long
edi_list_length (edi_list_t * this)
{
  return this->length;
}


void *
edi_list_exists (edi_list_t * this, void *key, edi_key_compare_t cmpfn)
{
  edi_node_t *node;

  for (node = this->first; node; node = node->next)
    if (cmpfn (node->key, key) == 0)
      return node;

  return NULL;
}

void *
edi_list_find (edi_list_t * this, void *key, edi_key_compare_t cmpfn)
{
  edi_node_t *node;

  if ((node = edi_list_exists (this, key, cmpfn)))
    return node->data;

  return NULL;
}




/**********************************************************************
 * Tree
 **********************************************************************/

void
edi_tree_init (edi_tree_t * this)
{
  this->root = NULL;
}

static void edi_tree_traverse
  (edi_node_t * node,
   void *user,
   edi_traverse_handler_t preorder,
   edi_traverse_handler_t inorder,
   edi_traverse_handler_t postorder,
   void (*nodefn) (void *), void (*datafn) (void *))
{

  if (preorder && node->used)
    preorder (user, node->key, node->data);

  if (node->prev)
    edi_tree_traverse (node->prev, user,
		       preorder, inorder, postorder, nodefn, datafn);

  if (inorder && node->used)
    inorder (user, node->key, node->data);

  if (node->next)
    edi_tree_traverse (node->next, user,
		       preorder, inorder, postorder, nodefn, datafn);

  if (postorder && node->used)
    postorder (user, node->key, node->data);

  if (datafn)
    datafn (node->data);

  if (nodefn)
    nodefn (node);
}


void
edi_tree_preorder (edi_tree_t * this, void *user, edi_traverse_handler_t h)
{
  if (this->root)
    edi_tree_traverse (this->root, user, h, NULL, NULL, NULL, NULL);
}

void
edi_tree_inorder (edi_tree_t * this, void *user, edi_traverse_handler_t h)
{
  if (this->root)
    edi_tree_traverse (this->root, user, NULL, h, NULL, NULL, NULL);
}

void
edi_tree_postorder (edi_tree_t * this, void *user, edi_traverse_handler_t h)
{
  if (this->root)
    edi_tree_traverse (this->root, user, NULL, NULL, h, NULL, NULL);
}


void
edi_tree_clear (edi_tree_t * this, edi_free_t freefn)
{
  if (this->root)
    edi_tree_traverse (this->root, NULL, NULL, NULL, NULL, free, freefn);

  this->root = NULL;
}

static edi_node_t *
edi_tree_findnode (edi_tree_t * this, unsigned long key, edi_node_t *** rptr)
{
  edi_node_t **root = NULL, *node = NULL;

  if (!this->root)
    root = &(this->root);
  else
    {
      node = this->root;

      while (node)
	{
	  if ((void *) key < node->key)
	    {
	      root = &(node->prev);
	      node = node->prev;
	    }
	  else if ((void *) key > node->key)
	    {
	      root = &(node->next);
	      node = node->next;
	    }
	  else
	    {
	      break;
	    }
	}
    }

  if (rptr)
    *rptr = root;
  return node;
}

int
edi_tree_insert (edi_tree_t * this, unsigned long key, void *data)
{
  edi_node_t **root = NULL, *node = NULL;

  if ((node = edi_tree_findnode (this, key, &root)))
    {
      if (node->used)
	return 0;

      node->used = 1;
      node->data = data;
    }
  else
    {
      if (!(node = edi_node_create ((void *) key, data)))
	return 0;
    }

  /* set the dangling pointer on the parent node (or tree root) */
  /* it may already be set, in which case this is a no-op */
  *root = node;

  /* success! */
  return 1;
}

void *
edi_tree_delete (edi_tree_t * this, unsigned long key)
{
  edi_node_t *node;
  void *data = NULL;

  if ((node = edi_tree_findnode (this, key, NULL)))
    {
      data = node->data;
      node->data = NULL;
      node->used = 0;
    }

  return data;
}

int edi_tree_replace
  (edi_tree_t * this, unsigned long key, void *data, edi_free_t freefn)
{
  void *old;

  if ((old = edi_tree_delete (this, key)) && freefn)
    freefn (old);

  return edi_tree_insert (this, key, data);
}

int
edi_tree_exists (edi_tree_t * this, unsigned long key)
{
  edi_node_t *node;
  node = edi_tree_findnode (this, key, NULL);

  if (!node)
    return 0;

  return node->used ? 1 : 0;
}

void *
edi_tree_find (edi_tree_t * this, unsigned long key)
{
  edi_node_t *node;
  node = edi_tree_findnode (this, key, NULL);

  if (!node)
    return NULL;

  return node->used ? node->data : NULL;
}




/**********************************************************************
 * Buffer
 **********************************************************************/

void
edi_buffer_init (edi_buffer_t * this)
{
  this->blck = 0;
  this->size = 0;
  this->data = NULL;
}

int
edi_buffer_append (edi_buffer_t * this, void *data, unsigned long size)
{
  void *ptr;
  unsigned long blck;

  if (!data || !size)
    return 1;

  if ((this->size + size + 1) > this->blck)
    {
      blck = ((this->size + size + 1) / 64) + 1;	/* + 1 for trailing '\0' */
      blck *= 64;
      ptr = this->data ? realloc (this->data, blck) : malloc (blck);
      if (ptr)
	this->data = ptr;
      else
	return 0;
    }

  memmove ((char *) this->data + this->size, (char *) data, size);
  this->size += size;
  /* mitigate unterminated data being used as a char pointer */
  *((char *) this->data + this->size) = '\0';
  return 1;
}

void
edi_buffer_clear (edi_buffer_t * this)
{
  free (this->data);
  this->blck = 0;
  this->size = 0;
  this->data = NULL;
}

unsigned long
edi_buffer_size (edi_buffer_t * this)
{
  return this->size;
}

void *
edi_buffer_data (edi_buffer_t * this)
{
  return this->data;
}




/**********************************************************************
 * Hash
 **********************************************************************/

static int
edi_hash_cmp (void *a, void *b)
{
  if (a < b)
    return -1;

  if (a > b)
    return 1;

  return 0;
}

static unsigned long
edi_hash_hash (void *v, unsigned int size)
{
  return (unsigned long) v;
}


int edi_hash_init
  (edi_hash_t * this, int size, edi_key_compare_t cmpfn,
   edi_key_hash_t hashfn)
{
  if (!(this->list = malloc (sizeof (edi_list_t) * size)))
    return 0;

  this->size = size;
  this->cmp = cmpfn ? cmpfn : edi_hash_cmp;
  this->hash = hashfn ? hashfn : edi_hash_hash;

  for (size = 0; size < this->size; size++)
    edi_list_init (this->list + size);

  return 1;
}

int
edi_hash_store (edi_hash_t * this, void *key, void *data)
{
  edi_list_t *list;
  unsigned int hash = this->hash (key, this->size);
  list = this->list + (hash % this->size);
  return edi_list_unshift_key (list, key, data);
}

void *
edi_hash_exists (edi_hash_t * this, void *key)
{
  edi_list_t *list;

  list = this->list + (this->hash (key, this->size) % this->size);
  return edi_list_exists (list, key, this->cmp);
}


void *
edi_hash_fetch (edi_hash_t * this, void *key)
{
  edi_node_t *node;

  if ((node = edi_hash_exists (this, key)))
    return node->data;

  return NULL;
}


void edi_hash_traverse
  (edi_hash_t * this, void *user, edi_traverse_handler_t handler)
{
  unsigned int n;
  edi_node_t *node;

  for (n = 0; n < this->size; n++)
    {
      node = (this->list + n)->first;
      while (node)
	{
	  handler (user, node->key, node->data);
	  node = node->next;
	}
    }
}

void
edi_hash_clear (edi_hash_t * this, edi_free_t freefn)
{
  int n;

  for (n = 0; n < this->size; n++)
    edi_list_clear (this->list + n, freefn);
}




/* Next three hash functions are courtesy of their various packages. */
/* Might not actually use these, but i've just put the code fragments */
/* here to remind me */


unsigned long
gtk_hash (void *v, unsigned int i)
{
  char *s = (char *) v;
  const char *p;
  unsigned long h = 0, g;

  for (p = s; *p != '\0'; p += 1)
    {
      h = (h << 4) + *p;
      if ((g = h & 0xf0000000))
	{
	  h = h ^ (g >> 24);
	  h = h ^ g;
	}
    }

  return h;
}


unsigned long
tcl_hash (void *v, unsigned int i)
{
  char *string = (char *) v;
  unsigned int result = 0;
  int c;

  while (1)
    {
      c = *string;
      string++;
      if (c == 0)
	{
	  break;
	}
      result += (result << 3) + c;
    }
  return result;
}


unsigned long
x31_hash (void *v, unsigned int i)
{
  char *p = v;
  unsigned long h = 0;
  for (; *p != '\0'; p += 1)
    {
      h = (h << 5) - h + *p;
    }
  return h;
}




/**********************************************************************
 * Unit tests
 **********************************************************************/

/* FIXME - write proper unit tests */
/* for now, if this doesn't bomb, assume it works */

static char *numbers[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen",
  "fourteen",
  "fifteen",
  "sixteen",
  "seventeen",
  "eighteen",
  "nineteen",
  "twenty",
};

static void
edi_adt_unit_tests_data (void *user, void *key, void *data)
{
  printf ("-%2ld: %s\n", (unsigned long) key, (char *) data);
}

static void
edi_adt_unit_tests_atad (void *user, void *key, void *data)
{
  edi_tree_t *tree = user;
  printf ("+%2ld: %s\n", (unsigned long) data, (char *) key);
  edi_tree_insert (tree, (unsigned long) data, key);
}

int
edi_adt_unit_tests (void)
{
  int n;
  edi_tree_t tree;
  edi_hash_t hash;

  edi_tree_init (&tree);
  edi_hash_init (&hash, 13, (int (*)(void *, void *)) strcmp, x31_hash);

  /* store numbers in the hash with the name of the number as key and
     the numeric value as data */

  for (n = 0; n < (sizeof (numbers) / sizeof (char *)); n++)
    edi_hash_store (&hash, numbers[n], (void *) n);

  /* traverse the hash, inserting records into the tree in a "random" order */
  edi_hash_traverse (&hash, &tree, edi_adt_unit_tests_atad);

  /* traverse the tree in order */
  edi_tree_inorder (&tree, NULL, edi_adt_unit_tests_data);

  edi_tree_clear (&tree, NULL);
  edi_hash_clear (&hash, NULL);

  return 1;
}
