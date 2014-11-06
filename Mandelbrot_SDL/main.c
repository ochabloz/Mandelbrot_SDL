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
#include "gfx.h"

// Size of the window
#define WIDTH 1280
#define HEIGHT 960

// Coordinates and size of the window within the Mandelbrot plane.
// For more examples of coordinates, check out http://www.cuug.ab.ca/dewara/mandelbrot/images.html
struct params_st {
   double xc;	// x position in the complex plane
   double yc;	// y position in the complex plane
   double size;	// size of the window in the complex plane
   long max_iter;	// maximum number of iterations
   double dcol;	// color increment (> 0)
};
typedef struct params_st params_t;

// Definition of a color map
struct colormap_st {
   uint32 *map;
   int length;
};
typedef struct colormap_st colormap_t;

/**
 * Create the colormap.
 * This tool can be used to visualize gradients: http://angrytools.com/gradient/
 */
void create_colormap(colormap_t *colmap) {
   int shade_count = 256;
   int step_count = 3;
   colmap->length = shade_count * step_count;
   colmap->map = (uint32 *) malloc(sizeof(uint32) * colmap->length);
   uint32 steps[] = { COLOR(6,88,189), COLOR(6,214,100), COLOR(255,99,133) };
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
               color = colmap->map[((int)((double)depth*p->dcol)) % colmap->length];
               break;
            }
         }
         gfx_setpix(surface, j, i, color);
         x += dx;
      }
      y += dy;
      
      // Every 32 lines: present surface to screen and check keyboard
      if (i & 32) {
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
      1.0E-11,
      8000,
      0.9 };
   
   /*
    // Longer computation
    params_t q = {
    -0.17476469999956,
    -1.0713151000007,
    5.095053e-13,
    8000,
    0.35 };
    
    // Classic coordinates
    params_t r = {
    -0.65,
    -0.0,
    1.2,
    150,
    10 };
    */
   
   mandelbrot(surface, &colmap, WIDTH, HEIGHT, &p);
   gfx_close();
   
   free_colormap(&colmap);
   
   return EXIT_SUCCESS;
}
