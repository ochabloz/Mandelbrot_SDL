/**
 * @file mandelbrot_seq.c
 * @brief Simple Mandelbrot fractal renderer.
 * @author Florent Gluck, florent.gluck@hesge.ch
 * @date November 2014
 * @version 0.1
 */

#define _GNU_SOURCE


typedef unsigned int uint;
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
   int i;
   int esc_has_been_pressed = 0;
   colormap_t colmap;
   create_colormap(&colmap);
   int nthread = 0;
   int nbloc = 0;
   int profile = 0;
   params_t parametres[] = {{ -0.65, -0.0, 1.2, 150, 10 }, // Classic coordinates
      {0.2929859127507, 0.6117848324958, 1.0E-12, 4000, 0.9 }, // Mandelbrot computation parameters
      {-0.17476469999956, -1.0713151001, 5.095053e-10, 20000, 0.9 }, //good one
      {0.35617945095070000, -0.6588216757098951155, 1/3.0550268E8,70000000000000, 3.5},
      {-1.278355973084,0.07390450051472 ,2.0/420.97435,1280,10}};
   
   
   for(int argi = 1; argi < argc; argi++)
   {
      if(!strncmp("--help",argv[argi],strlen("--help")))
      {
         printf("MANDELBROT\n");
         printf("compiled on " __TIMESTAMP__"\n");
         printf("Olivier Chabloz, Balthazar de Moncuit, Alexis Marquet\n\n\n");
         printf("Options:\n");
         printf("--nthread=[ARG]  \t # of threads\n");
         printf("--nblocs=[ARG]   \t # of blocs for the render (must be > nthread)\n");
         printf("--height=[ARG]   \t height resolution (default = 1080)\n");
         printf("--width=[ARG]    \t width resolution (default = 1920)\n");
         printf("--img-path=[ARG] \t path to save image to. if not specified, no image will be rendered\n");
         printf("--profile=[ARG]  \t profile to render (internal, 1 to %lu)\n",sizeof(parametres)/sizeof(params_t));
         exit(0);
      }
      else if (!strncmp("--nthread=",argv[argi],strlen("--nthread=")))
      {
         nthread = atoi(&argv[argi][strlen("--nthread=")]);
      }
      else if (!strncmp("--nbloc=",argv[argi],strlen("--nbloc=")))
      {
         nbloc = atoi(&argv[argi][strlen("--nbloc=")]);
      }
      else if(!strncmp("--height=",argv[argi],strlen("--height=")))
      {
         HEIGHT= atoi(&argv[argi][strlen("--height=")]);
      }
      else if(!strncmp("--width=",argv[argi],strlen("--width=")))
      {
         WIDTH = atoi(&argv[argi][strlen("--width=")]);
      }
      else if(!strncmp("--profile=",argv[argi],strlen("--profile=")))
      {
         profile = atoi(&argv[argi][strlen("--profile=")]);
         profile -= 1;
      }
   }
   if((nthread==0) || (nbloc ==0))
   {
      fprintf(stderr,"ERROR : please enter valid nthread & nbloc values. for help, option '--help'\n");
      exit(1);
   }
   if(nthread > nbloc)
   {
      fprintf(stderr,"ERROR : nthread must be < nbloc. for help, option '--help'\n");
      exit(1);
   }
   
   // Rendering surface
   SURFACE *surface = gfx_init("Mandelbrot", WIDTH, HEIGHT);
   if (surface == NULL) {
      fprintf(stderr, "Failed initializing video mode!\n");
      return EXIT_FAILURE;
   }
   

   
   pthread_t *mandelbrot_t = (pthread_t*) malloc(sizeof(pthread_t)*nthread);
   if(!mandelbrot_t)
   {
      fprintf(stderr, "error allocating memory for thread information\n");
      exit(1);
   }
   
   Pile_t *s;
   create_stack_from_surface(surface,&s, nbloc);
   info_mandelbrot_thread info;
   info.p = &parametres[profile];
   info.s = s;
   info.d = surface;
   info.c = &colmap;
   
   write_char_to_pos('a', 10, surface);
   clock_t start, end;
   start = clock();
   
   for (i = 0; i < nthread; i++) {
      if(pthread_create(&mandelbrot_t[i], NULL,Mandelbrot ,&info) != 0)
      {
         fprintf(stderr, "error creating the threads\n");
         exit(1);
      }
   }
   pthread_t refresher;
   pthread_create(&refresher, NULL, thread_is_escaped, &esc_has_been_pressed);

   while (!esc_has_been_pressed) {
      usleep(40000);
      gfx_present(surface);
   }
   
   end = clock();
   end = end-start;
   double elapsed = end / (double)CLOCKS_PER_SEC;
   printf("elapsed = %lf s\n",elapsed);
   char*str = malloc(3000);
   sprintf(str,"mandelbrot" __DATE__" "__TIME__ " %lE %lE %lE %ld %lE en %lf s.bmp",
           parametres[profile].xc,
           parametres[profile].yc,
           parametres[profile].size,
           parametres[profile].max_iter,
           parametres[profile].dcol,
           elapsed);
   int res = SDL_SaveBMP(surface->image,str);
   printf("%d res\n",res);
   gfx_close();
   
   free_colormap(&colmap);
   free(str);
   free (mandelbrot_t);
   return EXIT_SUCCESS;
}
