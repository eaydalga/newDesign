/* doc----------------------------------------------------------------
:Project.   common tools
:Program.   lists.c
:PrType.    function lib
:Title.     tools for list processing
:Version.   1.0
:Author.    Max Wildgrube
:Copyright.
:Language.  ANSI-C
:Compiler.
:Created.   1998-10-17 
----------------------------------------------------------------------*/

#include "lists.h"
#include <stdarg.h>
#include <stdio.h>

/*********************************x*******************************/ 
LI_List *LI_init (LI_List *p_list)
{
  if (! p_list) p_list = malloc (sizeof (LI_List));
  if (! p_list) abort ();

  p_list->head.next = &p_list->tail;
  p_list->tail.prev = &p_list->head;
  p_list->head.prev = NULL;
  p_list->tail.next = NULL;
  p_list->current   = NULL;

  return p_list;

}
/*end- LI_init */ 

/****************************************************************/ 
bool  LI_isEmpty    (LI_List *p_list)
{
  if (! p_list) return true;

  if (p_list->head.next->next) 
    return false;
  else
    return true;
}
/*end- LI_init */ 

/****************************************************************/ 
LI_Node *LI_getFirst (LI_List *p_list)
{
  if (! p_list)                  return NULL;
  p_list->current = NULL;
  if (! p_list->head.next)       return NULL;
  if (! p_list->head.next->next) return NULL;

  return p_list->current = p_list->head.next;
}
/*end- LI_getFirst */ 

/****************************************************************/ 
LI_Node *LI_getLast  (LI_List *p_list)
{
  if (! p_list)                  return NULL;
  p_list->current = NULL;
  if (! p_list->tail.prev)       return NULL;
  if (! p_list->tail.prev->prev) return NULL;

  return p_list->current = p_list->tail.prev;

}
/*end- LI_getLast */ 

/****************************************************************/ 
LI_Node *LI_getNext  (LI_List *p_list)
{
  if (! p_list)          return NULL;
  if (! p_list->current) return NULL;

  p_list->current = p_list->current->next;

  if (! p_list->current->next) p_list->current = NULL;

  return p_list->current;

}
/*end- LI_getNext */ 

/****************************************************************/ 
LI_Node *LI_getPrev  (LI_List *p_list)
{
  if (! p_list) return NULL;

  if (! p_list->current) return NULL;

  p_list->current = p_list->current->prev;

  if (! p_list->current->prev) p_list->current = NULL;

  return p_list->current;
}
/*end- LI_getPrev */ 


/****************************************************************/ 
bool LI_addHead (LI_List *p_list, LI_Node *p_node)
{
  if (! p_list) return false;
  if (! p_node) return false;

  p_node->next       = p_list->head.next;
  p_node->prev       = &p_list->head;
  p_node->next->prev = p_node; 
  p_list->head.next  = p_node;

  return true;
}
/*end- LI_addHead  */ 

/****************************************************************/ 
bool LI_addTail (LI_List *p_list, LI_Node *p_node)
{
  if (! p_list) return false;
  if (! p_node) return false;

  p_node->prev       = p_list->tail.prev;
  p_node->next       = &p_list->tail;
  p_node->prev->next = p_node; 
  p_list->tail.prev  = p_node;

  return true;

}
/*end- LI_addTail  */ 

/****************************************************************/ 
bool LI_setCurrent (LI_List *p_list, LI_Node *p_node)
{
  if (! p_list) return false;
  p_list->current = p_node;  
  return true;
}
/*end- LI_setCurrent  */ 

/****************************************************************/ 
bool LI_addBefore  (LI_Node *pold, LI_Node *p_new)
{
  if (! p_new      || ! pold)       return false;
  if (! pold->next || ! pold->prev) return false;

  pold->prev->next = p_new;
  p_new->prev      = pold->prev;
  p_new->next      = pold;
  pold->prev       = p_new;

  return true;

}
/*end- LI_addBefore  */ 

/****************************************************************/ 
bool LI_addBehind  (LI_Node *pold, LI_Node *p_new)
{
  if (! p_new       || ! pold)       return false;
  if (! pold->next || ! pold->prev) return false;

  pold->next->prev = p_new;
  p_new->next       = pold->next;
  p_new->prev       = pold;
  pold->next       = p_new;

  return true;

}
/*end- LI_addBehind  */ 

/****************************************************************/ 
bool LI_remove  (LI_Node *p_node)
{
  if (! p_node) return false;
  if (! p_node->next || ! p_node->prev) return false;

  p_node->prev->next = p_node->next;
  p_node->next->prev = p_node->prev;
  p_node->next       = NULL; 
  p_node->prev       = NULL; 

  return true;

}
/*end- LI_remove */ 

/****************************************************************/ 
LI_Node *LI_remCurrent (LI_List *p_list)
{
  LI_Node *node;

  if (LI_isEmpty (p_list)) return NULL;

  node = p_list->current;
  if (! node || ! node->next) return NULL;

  p_list->current = p_list->current->prev;

  return LI_remove (node) ? node : NULL;
}
/*end- LI_remCurrent */ 

/****************************************************************/ 
LI_Node *LI_remHead (LI_List *p_list)
{
  LI_Node *node;
  
  if (LI_isEmpty (p_list)) return NULL;

  node = p_list->head.next;
  LI_remove (node);

  return node;
}
/*end- LI_remHead */ 

/****************************************************************/ 
LI_Node *LI_remTail (LI_List *p_list)
{
  LI_Node *node;
  
  if (LI_isEmpty (p_list)) return NULL;

  node = p_list->tail.prev;
  LI_remove (node);

  return node;
}
/*end- LI_remTail */ 

/****************************************************************/ 
bool LI_append (LI_List *p_list1, LI_List *p_list2)
{
  if (LI_isEmpty (p_list2)) return false;
  
  p_list2->tail.prev->next = p_list1->tail.prev->next;
  p_list1->tail.prev->next = p_list2->head.next;
  p_list2->head.next->prev = p_list1->tail.prev;
  p_list1->tail.prev       = p_list2->tail.prev;

  LI_init (p_list2);
  return true;
}
/*end- LI_append */ 

/****************************************************************/ 
LI_Node *LI_doForward  (LI_List *p_list, LI_DoProc *p_proc)
{
  LI_Node *node;

  node = p_list->head.next;

  while (node->next)
  {
    if (p_proc (node)) return node;
    node = node->next;
  }  

  return NULL;
}
/*end- LI_doForward */ 

/****************************************************************/ 
LI_Node *LI_doBackward (LI_List *p_list, LI_DoProc *p_proc)
{
  LI_Node *node;

  node = p_list->tail.prev;

  while (node->prev)
  {
    if (p_proc (node)) return node;
    node = node->prev;
  }  

   return NULL;
}
/*end- LI_doBackward */ 


/****************************************************************/ 
/**                              SORT                          **/ 
/****************************************************************/ 


#define LESS(p1,p2)  !p2 ? false : !p1 ? true : p_Less(p1,p2)
 
/****************************************************************/ 
bool LI_sort (LI_List *p_list, LI_LessProc *p_Less)

/* new version 94-09-18 by max: simplified string sort */ 
/* adapted from OBERON-2 source on 98-10-18            */ 
{
  LI_Node  *node, *node0, *node1;
  LI_List   listA [2];
          
  int  ix, ndx;
  bool isLowerN0toN1, isLowerN0toL, isLowerN1toL;  /* boolean */ 
  
  if (p_Less == NULL)
  {
    printf ("program error LI_sort: 2nd param is Null!\n");
    exit(EXIT_FAILURE);
  }

  if ( LI_isEmpty (p_list)) return false;

  LI_init (&listA [0]);
  LI_init (&listA [1]);

  while (true)
  {
    /* 1. splitting */ 

    ix = 0;
    node0 = LI_remHead (p_list);
    node  = LI_remHead (p_list);
    LI_addTail (&listA [0], node0);

    while (node != NULL)
    {
      if (LESS (node, node0)) ix = 1 - ix;
      LI_addTail (&listA [ix], node);
      node0 = node;
      node  = LI_remHead (p_list);

    } /*END-WHILE node */ 

    /* p_list is empty now! */ 

    if (LI_isEmpty (&listA [1]) )
    {
      /* ok: Umhängen listA [0] nach list: */ 
      LI_append (p_list, &listA [0]);
      return true;
      /* ===== sorted ! ===========================>> */ 
    } /*END- if*/ 

    /* 2. merging */ 
    /*##############################################################
    one of N0 or N1 must be appended after p_list (=L). Which one? I
    compare each of N0, N1 and p_list with each other. (3 compares).
    the actual test are stored in the bools
    isLower...N0toN1, ...N0toL and ...N1toL

    there are 6 possible combinations:
    (action A --> B means: A ist appended to B)

    combination    action    N0 < N1   N0 < L  N1 < L  ndx  newRel
    ------------------------------------------------------------------
    L < N1 < N0   N1 --> L   F =  0    F = 0   F = 0   0    N0 > L
    L < N0 < N1   N0 --> L   T =  1    F = 0   F = 0   1    N1 > L
    N0 < L < N1   N1 --> L   T =  1    T = 2   F = 0   3    N0 < L
    N1 < L < N0   N0 --> L   F =  0    F = 0   T = 4   4    N1 < L
    N1 < N0 < L   N1 --> L   F =  0    T = 2   T = 4   6    N0 > L
    N0 < N1 < L   N0 --> L   T =  1    T = 2   T = 4   7    N1 > L

    ndx = 1*(N0<N1) + 2*(N0<L) + 4*(N1<L)  with TRUE=1 and FALSE=0
    ################################################################
    */ 

    node0 = LI_remHead (&listA [0]);
    node1 = LI_remHead (&listA [1]);

    isLowerN0toN1 = LESS (node0, node1);
    isLowerN0toL  = false;                 /* empty node is lower */ 
    isLowerN1toL  = false;                 /* than everything     */ 

    while ( (node0 != NULL) && (node1 != NULL))
    {
      if ( isLowerN0toN1 )  ndx  = 1; else ndx = 0;
      if ( isLowerN0toL  )  ndx += 2;
      if ( isLowerN1toL  )  ndx += 4;

      switch (ndx)
      {
        case 0 :
        case 6 :
          node         = node1;
          node1        = LI_remHead (&listA [1]);
          isLowerN0toL = false;
          isLowerN1toL = LESS (node1, node);
          break;

        case 1 :
        case 7 :
          node         = node0;
          node0        = LI_remHead (&listA [0]);
          isLowerN0toL = LESS (node0, node);
          isLowerN1toL = false;
          break;

        case 3 :
          node         = node1;
          node1        = LI_remHead (&listA [1]);
          isLowerN0toL = true;
          isLowerN1toL = LESS (node1, node);
          break;

        case 4 :
          node         = node0;
          node0        = LI_remHead (&listA [0]);
          isLowerN0toL = LESS (node0, node);
          isLowerN1toL = true;
          break;

      }/*END-switch*/ ;

      isLowerN0toN1 = LESS (node0, node1);
      LI_addTail (p_list, node);

    }/*END-WHILE*/ 

    /* einer der beiden restlichen Strings an List anhängen:  */ 

    if ( node0 != NULL && node1 == NULL )
    {
      LI_addTail (p_list, node0);
      LI_append  (p_list, &listA [0]);
    }
    else if ( node0 == NULL && node1 != NULL )
    {
      LI_addTail (p_list, node1);
      LI_append  (p_list, &listA [1]);

    } /*END-if (*/ 

    /***
    * if ( sorted ) EXIT END;  problem noch ungelöst:
    * kann ich beim Mischen schon erkennen ob schon sortiert ?
    ***/ 

  } /*END-LOOP*/ 

  return true;

} /*END- LI_sort*/ 

/*** end of source ****/ 

