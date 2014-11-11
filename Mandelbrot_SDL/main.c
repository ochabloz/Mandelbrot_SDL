/**
 * @file mandelbrot_seq.c
 * @brief Simple Mandelbrot fractal renderer.
 * @author Florent Gluck, florent.gluck@hesge.ch
 * @date November 2014
 * @version 0.1
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "gfx.h"
#include "Mandelbrot.h"
#include "stack.h"



/**
 * Program's entry point.
 * @param argc number of arguments.
 * @param arguments (as an array of strings).
 * @return status code.
 */
int main(int argc, char **argv) {
   colormap_t colmap;
   create_colormap(&colmap);
   
   // Rendering surface
   SURFACE *surface = gfx_init("Mandelbrot", WIDTH, HEIGHT);
   if (surface == NULL) {
      fprintf(stderr, "Failed initializing video mode!\n");
      return EXIT_FAILURE;
   }
   
   // Mandelbrot computation parameters
   params_t p = {
      0.2929859127507,
      0.6117848324958,
      1.0E-12,
      4000,
      0.9 };
   
   
     //Longer computation
    params_t t = {
    -0.17476469999956,
    -1.0713151001,
    5.095053e-10,
    4000,
    0.9 };
    
    // Classic coordinates
    params_t o = {
    -0.65,
    -0.0,
    1.2,
    150,
    10 };
   
   
   pthread_t * thread_refresh;
   
   //pthread_create(thread_refresh, NULL, thread_render_present, (void*)surface);
   
   //mandelbrot(surface, &colmap, WIDTH, HEIGHT, &t);
   Pile_t *s;
   create_stack_from_surface(surface,&s, 1);
   //create_stack(&s);
   //bloc_t b = {NULL,0,HEIGHT*WIDTH};
   //push_stack(s,&b);
   info_mandelbrot_thread i;
   i.p = &o;
   i.s = s;
   i.d = surface;
   Mandelbrot((void*)&i);
   gfx_close();
   
   free_colormap(&colmap);
   
   return EXIT_SUCCESS;
}
