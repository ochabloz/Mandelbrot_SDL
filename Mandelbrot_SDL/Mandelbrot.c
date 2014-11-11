//
//  Mandelbrot.c
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 06/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#include <stdio.h>
#include "Mandelbrot.h"
#include "gfx.h"
#include "stack.h"
#include <math.h>

void *thread_mandelbrot(void *arg)
{
/*   args_t *a = arg;  //struct
   
   bloc_t *b;
   unsigned int left;
   //lock stack
   while(1)
   {
      //lock stack
      left = isleft(p);
      if(left==0)
      {
         //unlock stack
         break;
      }
      b = pop(p);
      //unlock stack
      
   }
   */
   return NULL;
}

/**
 * Create the colormap.
 * This tool can be used to visualize gradients: http://angrytools.com/gradient/
 */
void create_colormap(colormap_t *colmap) {
   int shade_count = 256;
   int step_count = 5;
   colmap->length = shade_count * step_count;
   colmap->map = (uint32 *) malloc(sizeof(uint32) * colmap->length);
   uint32 steps[] = { COLOR(0,7,100), COLOR(32,107,203), COLOR(255,255,255), COLOR(255,170,0), COLOR(0,2,0) };
   int c = 0;
   for (int j = 0; j < step_count; j++) {
      double dr = ((double)COLOR_GET_R(steps[(j+1) % step_count]) - (double)COLOR_GET_R(steps[j]))/(double)shade_count;
      double dg = ((double)COLOR_GET_G(steps[(j+1) % step_count]) - (double)COLOR_GET_G(steps[j]))/(double)shade_count;
      double db = ((double)COLOR_GET_B(steps[(j+1) % step_count]) - (double)COLOR_GET_B(steps[j]))/(double)shade_count;
      for (int i = 0; i < shade_count; i++) {
         uint8 r = (uint8)((double)COLOR_GET_R(steps[j]) + dr * (double)i);
         uint8 g = (uint8)((double)COLOR_GET_G(steps[j]) + dg * (double)i);
         uint8 b = (uint8)((double)COLOR_GET_B(steps[j]) + db * (double)i);
         colmap->map[c++] = COLOR(r,g,b);
      }
   }
}

/**
 * Free the memory used by the color map.
 */
void free_colormap(colormap_t *colmap) {
   free(colmap->map);
   colmap->map = 0;
}

/**
 * Render the Mandelbrot setup.
 */
void mandelbrot(SURFACE *surface, colormap_t *colmap, uint width, uint height, params_t *p) {
   double x1 =  p->xc - p->size;
   double x2 =  p->xc + p->size;
   double y1 =  p->yc - p->size;
   double y2 =  p->yc + p->size;
   double dx = (x2 - x1) / width;
   double dy = (y2 - y1) / height;
   double y = y1;
   
   for (int i = 0; i < height; i++) {
      double x = x1;
      
      for (int j = 0; j < width; j++) {
         double zx = 0;
         double zy = 0;
         uint32 color = COLOR(0,0,0);
         
         for (long depth = 0; depth < p->max_iter; depth++) {
            double zx_new = (zx*zx) - (zy*zy) + x;
            double zy_new = 2.0*zx*zy + y;
            zx = zx_new;
            zy = zy_new;
            // Did the pixel diverge (go to infinity)?
            if ((zx*zx + zy*zy) > 4.0) {
               //double dist = log(sqrt(zx*zx+zy*zy))/pow(2,depth);
               //color = colmap->map[((int)((double)dist*depth*p->dcol)) % colmap->length];
               color = colmap->map[((int)((double)depth*p->dcol)) % colmap->length];
               break;
            }
         }
         gfx_setpix(surface, j, i, color);
         x += dx;
      }
      y += dy;
      
      // Every 32 lines: present surface to screen and check keyboard
      if ((i & 31) == 0) {
         gfx_present(surface);
         if (gfx_is_esc_pressed()) {
            return;
         }
         
      }
   }
   while (!gfx_is_esc_pressed()) {
      usleep(100000);  // Check every 0.1 sec. (10 Hz)
   }
}

void *Mandelbrot(void *arg)
{
   info_mandelbrot_thread *info = (info_mandelbrot_thread*)arg;
   bloc_t *actual;
   //lock param_t info->p
   params_t p;
   memcpy(&p,info->p,sizeof(params_t)-sizeof(SPINLOCK_T));
   //unlock info->p
   while(1)
   {
      //lock pile
      //lock_stack(info->s);
      // if pile != empty
      if(!is_stack_empty(info->s))
      {
         // get_pile
         pop_stack(info->s,(void**)&actual);
         // unlock
         //unlock_stack(info->s);
         // process mandelbrot
         
         long i = (actual->index)%WIDTH;
         long j = (actual->index)/WIDTH;
         
         long j_prev = actual->index/WIDTH;
         
         double x1 =  p.xc - p.size;
         double x2 =  p.xc + p.size;
         double y1 =  p.yc - p.size;
         double y2 =  p.yc + p.size;
         double dx = (x2 - x1) / WIDTH;
         double dy = (y2 - y1) / WIDTH;
         double y = y1 + j*dy;
         double x = x1 + i*dx;
         
         int color = 0;
         
         for(int k = 0; k < actual->n;k++)
         {
            i = (actual->index+k)%WIDTH;
            j = (actual->index+k)/WIDTH;
            double zx = 0,zy = 0;
            for(long depth = 0; depth < p.max_iter; depth ++)
            {
               double zx_new = (zx*zx) - (zy*zy) + x;
               double zy_new = 2.0*zx*zy + y;
               zx = zx_new;
               zy = zy_new;
               // Did the pixel diverge (go to infinity)?
               if ((zx*zx + zy*zy) > 4.0) {
                  int c = (int)255*((float)depth/(float)p.max_iter);
                  //colmap->map[((int)((double)depth*p->dcol)) % colmap->length];
                  color = c | (c<<8) | (c<<16);
                  break;
               }
            }
            if(j_prev != j)
            {
               y += dy;
            }
            gfx_setpix(info->d, i,j, color);
            
         }
      }
   // else
      else
      {
         break;
      }
   //    unlock
   //    quit
   }

   
   
   return NULL;
}
