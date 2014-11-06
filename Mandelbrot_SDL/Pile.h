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

typedef struct Pile Pile;

struct Pile{
   int nb_elements;
   bloc_t * bloc;
};

void pop(Pile* p, bloc_t** bloc);
void push(Pile* p, bloc_t** bloc);
int is_stack_empty(Pile* p);
#endif
