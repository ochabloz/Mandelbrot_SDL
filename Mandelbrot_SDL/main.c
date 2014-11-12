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
#include <time.h>



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
   
   
    // Classic coordinates
    params_t o = {
    -0.65,
    -0.0,
    1.2,
    150,
    10 };
   
   
   
  //pthread_create(thread_refresh, NULL, thread_render_present, (void*)surface);
   
   //mandelbrot(surface, &colmap, WIDTH, HEIGHT, &p);
   
   
   
   // Mandelbrot computation parameters
   params_t p = {
      0.2929859127507,
      0.6117848324958,
      1.0E-12,
      4000,
      0.9 };
   
   
   //Longer computation
   /*params_t t = {
      -0.72,
      -0.2003,
      0.0000000003,
      1000000,
      0.18 };*/
   //good one:
   params_t t = {
      -0.17476469999956,
      -1.0713151001,
      5.095053e-10,
      20000,
      0.9 };

   //deep
   /*params_t t = {
    -1.2539860994437551993560939157209647078063,
    -0.02507217260555307457626152143640762062285,
    //4.50359962737E15,
    //(1.0/4.50359962737E12),
    2.220446049250557627727881876774274745181188004E-12L,
    20000000,
    0.15L };*/
   
   Pile_t *s;
   create_stack_from_surface(surface,&s, 1);
   info_mandelbrot_thread i;
   i.p = &t;
   i.s = s;
   i.d = surface;
   i.c = &colmap;
   
   pthread_t  thread_refresh;
   clock_t start, end;
   start = clock();
   pthread_create(&thread_refresh, NULL,thread_render_present ,surface);
   Mandelbrot((void*)&i);
   end = clock();
   end = end-start;
   double elapsed = end / (double)CLOCKS_PER_SEC;
   printf("elapsed = %lf s\n",elapsed);
   char*str = malloc(3000);
   sprintf(str,"mandelbrot" __DATE__" "__TIME__ " %lE %lE %lE %ld %lE en %lf s.bmp",t.xc,t.yc,t.size,t.max_iter,t.dcol, elapsed);
   int res = SDL_SaveBMP(surface->image,str);
   printf("%d res\n",res);
   gfx_close();
   
   free_colormap(&colmap);
   free(str);
   return EXIT_SUCCESS;
}
