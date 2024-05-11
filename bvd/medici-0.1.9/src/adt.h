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

#ifndef ADT_H
#define ADT_H

typedef int (*edi_key_compare_t) (void *, void *);
typedef unsigned long (*edi_key_hash_t) (void *, unsigned int);
typedef void (*edi_traverse_handler_t) (void *, void *, void *);
typedef void (*edi_free_t) (void *);


typedef struct edi_node_s edi_node_t;
typedef struct
{
  unsigned long length;
  edi_node_t *first;
  edi_node_t *last;
}
edi_list_t;

typedef edi_list_t edi_stack_t;
typedef edi_list_t edi_queue_t;

typedef struct
{
  edi_node_t *root;
}
edi_tree_t;

typedef struct
{
  unsigned long blck;
  unsigned long size;
  void *data;
}
edi_buffer_t;

typedef struct
{
  unsigned int size;
  edi_list_t *list;
  edi_key_compare_t cmp;
  edi_key_hash_t hash;
}
edi_hash_t;

struct edi_node_s
{
  void *key;
  void *data;
  edi_node_t *prev;
  edi_node_t *next;
  int used;
};


#define edi_stack_init  edi_list_init
#define edi_stack_clear edi_list_clear
#define edi_stack_push  edi_list_push
#define edi_stack_peek  edi_list_peek
#define edi_stack_pop   edi_list_pop
#define edi_stack_size  edi_list_length

#define edi_queue_init     edi_list_init
#define edi_queue_drain    edi_list_drain
#define edi_queue_queue    edi_list_push
#define edi_queue_dequeue  edi_list_shift
#define edi_queue_unqueue  edi_list_pop
#define edi_queue_length   edi_list_length




void edi_list_init (edi_list_t * this);
int edi_list_push (edi_list_t * this, void *data);
int edi_list_push_key (edi_list_t * this, void *key, void *data);
void *edi_list_pop (edi_list_t * this);
void *edi_list_peek (edi_list_t * this);
void *edi_list_shift (edi_list_t * this);
int edi_list_unshift (edi_list_t * this, void *data);
int edi_list_unshift_key (edi_list_t * this, void *key, void *data);
void *edi_list_head (edi_list_t * this);
void edi_list_drain (edi_list_t * this, edi_free_t freefn);
void edi_list_clear (edi_list_t * this, edi_free_t freefn);
unsigned long edi_list_length (edi_list_t * this);
void *edi_list_exists (edi_list_t * this, void *key, edi_key_compare_t cmpfn);
void *edi_list_find (edi_list_t * this, void *key, edi_key_compare_t cmpfn);



void edi_tree_init (edi_tree_t * this);
void edi_tree_preorder (edi_tree_t * this, void *user,
			edi_traverse_handler_t h);
void edi_tree_inorder (edi_tree_t * this, void *user,
		       edi_traverse_handler_t h);
void edi_tree_postorder (edi_tree_t * this, void *user,
			 edi_traverse_handler_t h);
void edi_tree_clear (edi_tree_t * this, void (*freefn) (void *));
int edi_tree_insert (edi_tree_t * this, unsigned long key, void *data);
void *edi_tree_delete (edi_tree_t * this, unsigned long key);
int edi_tree_replace (edi_tree_t * this, unsigned long key, void *data,
		      edi_free_t freefn);
int edi_tree_exists (edi_tree_t * this, unsigned long key);
void *edi_tree_find (edi_tree_t * this, unsigned long key);



void edi_buffer_init (edi_buffer_t * this);
int edi_buffer_append (edi_buffer_t * this, void *data, unsigned long size);
void edi_buffer_clear (edi_buffer_t * this);
unsigned long edi_buffer_size (edi_buffer_t * this);
void *edi_buffer_data (edi_buffer_t * this);



int edi_hash_init (edi_hash_t * this, int size, edi_key_compare_t cmpfn,
		   edi_key_hash_t hashfn);
int edi_hash_store (edi_hash_t * this, void *key, void *data);
void *edi_hash_exists (edi_hash_t * this, void *key);
void *edi_hash_fetch (edi_hash_t * this, void *key);
void edi_hash_traverse (edi_hash_t * this, void *user,
			edi_traverse_handler_t handler);
void edi_hash_clear (edi_hash_t * this, void (*freefn) (void *));



unsigned long gtk_hash (void *v, unsigned int i);
unsigned long tcl_hash (void *v, unsigned int i);
unsigned long x31_hash (void *v, unsigned int i);
int edi_adt_unit_tests (void);

#endif /*ADT_H */
