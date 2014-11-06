//
//  Pile.h
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 06/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef Mandelbrot_SDL_Pile_h
#define Mandelbrot_SDL_Pile_h
#include "Mandelbrot.h"
#include "gfx.h"

typedef struct Pile Pile;
typedef struct Pile_elem Pile_elem;

struct Pile{
   int nb_elements;
   Pile_elem * suivant;
};



struct Pile_elem{
   void* element;
   Pile_elem * suivant;
};

void pop_stack(Pile* p, void** bloc);
void push_stack(Pile* p, void* bloc);
int is_stack_empty(Pile* p);

void create_stack_from_surface(SURFACE * s, Pile ** stack, Uint32 nb_blocs);

#endif
