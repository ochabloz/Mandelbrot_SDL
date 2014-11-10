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
#include "spinlock.h"

/**
 * push the generic type bloc into a stack Pile_t
 * @param p pile (stack) of type Pile_t
 * @param bloc the element to store
 */
void push_stack(Pile_t* p, void* bloc){
   // create an internal element
   Pile_elem * new_item = malloc(sizeof(Pile_elem));
   if(!new_item)
   {
      fprintf(stderr, "error allocating memory for stack element\n");
      exit(1);
   }
   // store generic bloc into the newly created Pile_elem
   new_item->element = bloc;
   // Push
   new_item->suivant = p->suivant;
   p->suivant = new_item;
   
   p->nb_elements++;
}

/**
 * retrive the top element from the stack
 * @param p pile (stack) of type Pile_t
 * @param bloc the element to retrieve
 */
void pop_stack(Pile_t* p, void** bloc){
   Pile_elem * to_trash;
   if (!is_stack_empty(p)) {
      *bloc = (p->suivant->element); // return bloc

      to_trash = p->suivant;
      
      // promote the 2nd element to 1st position
      if (p->nb_elements > 1)
         p->suivant = p->suivant->suivant;
      else p->suivant = NULL;
      p->nb_elements--;
      free(to_trash); // free old 1st elem
   }
   else *bloc = NULL;
}

/**
 * check if the stack is empty
 * @param p pile (stack) of type Pile_t
 * @return 1 if empty, 0 if not empty
 */
int is_stack_empty(Pile_t* p){
   return p->nb_elements <= 0;
}

/**
 * Create and initiate a stack of type Pile_t
 * @param stack of type Pile_t
 * @param bloc the element to store
 */
void create_stack(Pile_t** stack){
   *stack = malloc(sizeof(Pile_t));
   if(!(*stack))
   {
      fprintf(stderr, "error allocating stack memory\n");
      exit(1);
   }
   (*stack)->nb_elements = 0;
   (*stack)->suivant = NULL;
   (*stack)->lock = INIT_SPINLOCK(&(*stack)->lock,0);
}

/**
 * Lock a stack
 * @param p the stack to lock
 */
void lock_stack(Pile_t* p){
   lock_spin(&(p->lock));
}

/**
 * Unlock a stack
 * @param p the stack to unlock
 */
void unlock_stack(Pile_t* p){
   unlock_spin(&(p->lock));
}

/**
 * Free a stack
 * @param stack the stack 2 be free
 */
void free_stack(Pile_t** stack){
   void * to_trash;
   while (!is_stack_empty(*stack)){
      pop_stack(*stack, &to_trash);
      free(to_trash);
   }
   free(*stack);
   *stack = NULL;
}

/**
 * Create a stack, divite the surface and push it into the stack
 * @param s the surface to divide
 * @param stack the stack to store the suface divided into blocs
 * @param nb_blocs the surface will be divided into nb_blocs
 */
void create_stack_from_surface(SURFACE * s, Pile_t** stack, Uint32 nb_blocs){

   Uint32 range, sum = 0;
   bloc_t *bloc_temp = NULL;
   create_stack(stack);
   
   for (int i = 0; i < nb_blocs; i++) {
      range = ((WIDTH * HEIGHT) - sum)/(nb_blocs-i);
      bloc_temp = malloc(sizeof(bloc_t));
      if(!bloc_temp)
      {
         fprintf(stderr, "error allocating memory for bloc (in create_stack_from_surface\n");
         exit(1);
      }
      bloc_temp->n = range;
      bloc_temp->s = s->image->pixels + sum * s->image->pitch / (DEPTH / 8);
      bloc_temp->index = sum;
      push_stack(*stack ,bloc_temp);
      sum += range; // actualise les valeurs pour la prochaine assignation
   }
}
