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
#include "gfx.h"

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
      free(to_trash);
   }
   else *bloc = NULL;
}

int is_stack_empty(Pile* p){
   return p->nb_elements <= 0;
}

void create_stack_from_surface(SURFACE * s, Pile ** stack, Uint32 nb_blocs){
   int width, height;
   Uint32 range, sum = 0, nb_pixel;
   bloc_t *bloc_temp = NULL;
   *stack = malloc(sizeof(Pile));
   
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