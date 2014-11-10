//
//  Pile.h
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 06/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef Mandelbrot_SDL_Pile_h
#define Mandelbrot_SDL_Pile_h
#include "gfx.h"

typedef struct Pile_t Pile_t;
typedef struct Pile_elem Pile_elem;

struct Pile_t{
   int nb_elements;
   Pile_elem * suivant;
};

struct Pile_elem{
   void* element;
   Pile_elem * suivant;
};

void pop_stack(Pile_t* p, void** bloc);
void push_stack(Pile_t* p, void* bloc);
int is_stack_empty(Pile_t* p);

void create_stack_from_surface(SURFACE * s, Pile_t ** stack, Uint32 nb_blocs);

#endif
