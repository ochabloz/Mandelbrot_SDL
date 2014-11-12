//
//  Mandelbrot.h
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 06/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef Mandelbrot_SDL_Mandelbrot_h
#define Mandelbrot_SDL_Mandelbrot_h
#include "stack.h"
#include "gfx.h"
#include "spinlock.h"


// Coordinates and size of the window within the Mandelbrot plane.
// For more examples of coordinates, check out http://www.cuug.ab.ca/dewara/mandelbrot/images.html
struct params_st {
   double xc;	// x position in the complex plane
   double yc;	// y position in the complex plane
   double size;	// size of the window in the complex plane
   long max_iter;	// maximum number of iterations
   double dcol;	// color increment (> 0)
   SPINLOCK_T lock;
};
typedef struct params_st params_t;

// Definition of a color map
struct colormap_st {
   uint32 *map;
   int length;
};
typedef struct colormap_st colormap_t;

typedef struct
{
   unsigned int *s;
   unsigned int index;
   unsigned int n;
}bloc_t;

typedef struct
{
   params_t * p;
   Pile_t * s;
   SURFACE *d;
   colormap_t *c;
}info_mandelbrot_thread;


void mandelbrot(SURFACE *surface, colormap_t *colmap, uint width, uint height, params_t *p);
void create_colormap(colormap_t *colmap);
void free_colormap(colormap_t *colmap);
void *Mandelbrot(void *arg);

#endif