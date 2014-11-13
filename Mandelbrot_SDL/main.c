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

#define PARAM_NUM 0

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
   
   params_t parametres[3] = {{ -0.65, -0.0, 1.2, 150, 10 }, // Classic coordinates
      {0.2929859127507, 0.6117848324958, 1.0E-12, 4000, 0.9 }, // Mandelbrot computation parameters
      {-0.17476469999956, -1.0713151001, 5.095053e-10, 20000, 0.9 }}; //good one
   
   
   Pile_t *s;
   create_stack_from_surface(surface,&s, 150);
   info_mandelbrot_thread i;
   i.p = &parametres[PARAM_NUM];
   i.s = s;
   i.d = surface;
   i.c = &colmap;
   
   pthread_t  thread_refresh, Mandel_t, mandela, mandelb;
   clock_t start, end;
   start = clock();
   pthread_create(&Mandel_t, NULL,Mandelbrot ,&i);
   pthread_create(&thread_refresh, NULL,Mandelbrot ,&i);
   pthread_create(&mandela, NULL,Mandelbrot ,&i);
   pthread_create(&mandelb, NULL,Mandelbrot ,&i);
   while (!gfx_is_esc_pressed()) {
      usleep(40000);
      gfx_present(surface);
   }
   
   end = clock();
   end = end-start;
   double elapsed = end / (double)CLOCKS_PER_SEC;
   printf("elapsed = %lf s\n",elapsed);
   char*str = malloc(3000);
   sprintf(str,"mandelbrot" __DATE__" "__TIME__ " %lE %lE %lE %ld %lE en %lf s.bmp",
           parametres[PARAM_NUM].xc,
           parametres[PARAM_NUM].yc,
           parametres[PARAM_NUM].size,
           parametres[PARAM_NUM].max_iter,
           parametres[PARAM_NUM].dcol,
           elapsed);
   int res = SDL_SaveBMP(surface->image,str);
   printf("%d res\n",res);
   gfx_close();
   
   free_colormap(&colmap);
   free(str);
   return EXIT_SUCCESS;
}
