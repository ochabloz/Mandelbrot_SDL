//
//  stack.c
//  Mandelbrot_SDL
//
//  Created by Olivier on 10.11.14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include "Mandelbrot.h"
#include "gfx.h"

void push_stack(Pile_t* p, void* bloc){
   Pile_elem * new_item = malloc(sizeof(Pile_elem));
   new_item->element = bloc;
   new_item->suivant = p->suivant;
   p->suivant = new_item;
   p->nb_elements++;
}

void pop_stack(Pile_t* p, void** bloc){
   Pile_elem * to_trash;
   if (!is_stack_empty(p)) {
      *bloc = &(p->suivant->element);
      to_trash = p->suivant;
      if (p->nb_elements > 1)
         p->suivant = p->suivant->suivant;
      else p->suivant = NULL;
      p->nb_elements--;
      free(to_trash);
   }
   else *bloc = NULL;
}

int is_stack_empty(Pile_t* p){
   return p->nb_elements <= 0;
}
void create_stack(Pile_t** stack){
   *stack = malloc(sizeof(Pile_t));
}

void lock_stack(Pile_t* p){
#ifdef __APPLE__
   OSSpinLockLock(&(p->lock));
#else
   pthread_spin_lock(&(p->lock));
#endif
}
void unlock_stack(Pile_t* p){
#ifdef __APPLE__
   OSSpinLockUnlock(&(p->lock));
#else
   pthread_spin_unlock(&(p->lock));
#endif
}

void create_stack_from_surface(SURFACE * s, Pile_t** stack, Uint32 nb_blocs){
   int width, height;
   Uint32 range, sum = 0, nb_pixel;
   bloc_t *bloc_temp = NULL;
   create_stack(stack);
   
   SDL_GetWindowSize(s->window, &width, &height);
   nb_pixel = width * height;
   for (int i = 0; i < nb_blocs; i++) {
      range = (nb_pixel - sum)/(nb_blocs-i);
      bloc_temp = malloc(sizeof(bloc_t));
      bloc_temp->n = range;
      bloc_temp->s = s->image->pixels + sum * s->image->pitch / (DEPTH / 8);
      push_stack(*stack ,bloc_temp);
      sum += range; // actualise les valeurs pour la prochaine assignation
   }
}