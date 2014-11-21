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

struct t_master_arg
{
   int nbloc;
   int nthread;
   int profile;
   SURFACE *surface;
};
void *Thread_Master(void* arg);


pthread_barrier_t bar;
int nthread = 0;

/**
 * Program's entry point.
 * @param argc number of arguments.
 * @param arguments (as an array of strings).
 * @return status code.
 */
int main(int argc, char **argv) {

   int nbloc = 0;
   int profile = 0; // the cell of parametres array
   

   
   
   // Arguments processing
   for(int argi = 1; argi < argc; argi++)
   {
      // if argument "--help" :
      // print all the options available and then exit the program
      //
      if(!strncmp("--help",argv[argi],strlen("--help")))
      {
         printf("MANDELBROT\n");
         printf("compiled on " __TIMESTAMP__"\n");
         printf("Olivier Chabloz, Balthazar de Moncuit, Alexis Marquet\n\n\n");
         printf("Options:\n");
         printf("--nthread=[ARG]  \t # of threads\n");
         printf("--nbloc=[ARG]   \t # of blocs for the render (must be > nthread)\n");
         printf("--height=[ARG]   \t height resolution (default = 1080)\n");
         printf("--width=[ARG]    \t width resolution (default = 1920)\n");
         printf("--img-path=[ARG] \t path to save image to. if not specified, no image will be rendered\n");
         printf("--profile=[ARG]  \t profile to render (internal, 1 to 5)\n");
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
         if(profile > 5)
         {
            perror("profile doesnt exist");
            return EXIT_FAILURE;
         }
      }
   } // end of Argument processing
   
   // EXIT if no '--nbloc' and no '--nthread' were entered
   if((nthread==0) || (nbloc ==0))
   {
      fprintf(stderr,"ERROR : please enter valid nthread & nbloc values. for help, option '--help'\n");
      exit(1);
   }
   
   // EXIT if the number of thread is greater than the number of blocs.
   if(nthread > nbloc)
   {
      fprintf(stderr,"ERROR : nthread must be < nbloc. for help, option '--help'\n");
      exit(1);
   }
   
   //init sdl
   SURFACE *surface = gfx_init("Mandelbrot", WIDTH, HEIGHT);
   if (surface == NULL) {
      fprintf(stderr, "Failed initializing video mode!\n");
      return EXIT_FAILURE;
   }
   //launch Thread esc(void)
   pthread_t t_esc;
   if(pthread_create(&t_esc, NULL, thread_is_escaped,NULL) !=0)
   {
      perror("error creating esc thread");
      return EXIT_FAILURE;
   }
   //launch Thread present(surface)
   pthread_t t_present;
   if(pthread_create(&t_present,NULL, thread_render_present,surface) != 0)
   {
      perror("error creting present thread");
      return EXIT_FAILURE;
   }
   //lanch thread Master
   pthread_t t_master;
   struct t_master_arg arg = {nbloc,nthread,profile,surface};
   pthread_barrier_init(&bar,NULL,2);
   if(pthread_create(&t_master, NULL, Thread_Master,&arg) != 0)
   {
      perror("error creating master thread");
      return EXIT_FAILURE;
   }
   //join (esc)
   if(pthread_join(t_esc,NULL) != 0)
   {
      perror("error joining esc thread");
   }
   //sync with master thread to kill the workers
   pthread_barrier_wait(&bar);
   pthread_barrier_destroy(&bar);
   if(pthread_join(t_master,NULL)!=0)
   {
      perror("error joining master thread");
   }
   //kill all
   pthread_cancel(t_present);
   gfx_close();
   //exit
   return EXIT_SUCCESS;
   }

void *Thread_Master(void *param)
{
   struct t_master_arg *arg = param;
   params_t parametres[] = {{ -0.65, -0.0, 1.2, 150, 10 }, // Classic coordinates
      {0.2929859127507, 0.6117848324958, 1.0E-12, 4000, 0.9 }, // Mandelbrot computation parameters
      {-0.17476469999956, -1.0713151001, 5.095053e-10, 20000, 0.9 }, //good one
      {0.35617945095070000, -0.6588216757098951155, 1/3.0550268E8,70000000000000, 3.5},
      {-1.278355973084,0.07390450051472 ,2.0/420.97435,1280,10}};
   
   //creating color map
   colormap_t colormap;
   create_colormap(&colormap);
   
   //creating stack
   Pile_t *s;
   create_stack_from_surface(arg->surface,&s, arg->nbloc);
   
   //creating mandelbrot thread info structure
   info_mandelbrot_thread info;
   info.p = &parametres[arg->profile];
   info.s = s;
   info.d = arg->surface;
   info.c = &colormap;
   gfx_print("salut", arg->surface);   
   //allocating thread structure memory
   pthread_t *mandelbrot_t = (pthread_t*) malloc(sizeof(pthread_t)*arg->nthread);
   if(!mandelbrot_t) {
      perror("error allocating memory for thread information");
      exit(1);
   }
   //creating mandelbrot threads
   for (int i = 0; i < arg->nthread; i++) {
      if(pthread_create(&mandelbrot_t[i], NULL,Mandelbrot ,&info) != 0)
      {
         fprintf(stderr, "error creating mandelbrot threads\n");
         exit(1);
      }
   }
   //waiting for esc to be pressed
   pthread_barrier_wait(&bar);
   for(int i = 0; i < arg->nthread; i++)
   {
      pthread_cancel(mandelbrot_t[i]);
   }
   free_stack(&s);
   free (mandelbrot_t);
   return NULL;
}


