//
//  Pile.c
//  Mandelbrot_SDL
//
//  Created by Olivier on 06.11.14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Pile.h"

void push_stack(Pile* p, void* bloc){
   Pile_elem * new_item = malloc(sizeof(Pile_elem));
   new_item->element = bloc;
   new_item->suivant = p->suivant;
   p->suivant = new_item;
   p->nb_elements++;
}

void pop_stack(Pile* p, void** bloc){
   Pile_elem * to_trash;
   if (!is_stack_empty(p)) {
      *bloc = &(p->suivant->element);
      to_trash = p->suivant;
      if (p->nb_elements > 1)
         p->suivant = p->suivant->suivant;
      else p->suivant = NULL;
      p->nb_elements--;
   }
   else *bloc = NULL;
}

int is_stack_empty(Pile* p){
   return p->nb_elements > 0;
}