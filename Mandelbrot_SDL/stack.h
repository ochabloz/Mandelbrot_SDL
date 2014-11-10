//
//  stack.h
//  Mandelbrot_SDL
//
//  Created by Olivier on 10.11.14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef __Mandelbrot_SDL__stack__
#define __Mandelbrot_SDL__stack__

#include "gfx.h"
#include <pthread.h>

typedef struct Pile_t Pile_t;
typedef struct Pile_elem Pile_elem;

struct Pile_t{
   int nb_elements;
   Pile_elem * suivant;
   pthread_rwlock_t * lock;
};

struct Pile_elem{
   void* element;
   Pile_elem * suivant;
};

void pop_stack(Pile_t* p, void** bloc);
void push_stack(Pile_t* p, void* bloc);
void lock_stack(Pile_t* p);
void unlock_stack(Pile_t* p);
int is_stack_empty(Pile_t* p);

void create_stack_from_surface(SURFACE * s, Pile_t ** stack, Uint32 nb_blocs);


#endif /* defined(__Mandelbrot_SDL__stack__) */
