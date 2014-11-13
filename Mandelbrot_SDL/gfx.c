/**
 * @file gfx.c
 * @brief Minimalistic SDL wrapper providing pixel drawing functions.
 * @author Florent Gluck, florent.gluck@hesge.ch
 * @date October, 2014
 * @version 0.1
 */

#include "gfx.h"

#define NB_ROW 2
#define NB_COL 20
#define CHAR_PIX_W 5
#define CHAR_PIX_H 5

/**
 * Compute pixel address for coordinates (x,y).
 * @param surface for which to compute the position.
 * @param x coordinate.
 * @param y coordinate.
 * @return pixel address.
 */
static uint32 *pix_addr(SURFACE *surface, int x, int y) {
   return (uint32 *) surface->image->pixels + y * surface->image->pitch / (DEPTH / 8) + x;
}

/**
 * Set pixel color at coordinates (x,y).
 * @param surface for which to draw the pixel.
 * @param x coordinate.
 * @param y coordinate.
 * @param col pixel color (RGB).
 */
void gfx_setpix(SURFACE *surface, int x, int y, uint32 col) {
   uint32 *pix = pix_addr(surface, x, y);
   *pix = SDL_MapRGB(surface->image->format, COLOR_GET_R(col), COLOR_GET_G(col), COLOR_GET_B(col));
}

/**
 * Get pixel color at coordinates (x,y).
 * @param surface for which to read the pixel.
 * @param x coordinate.
 * @param y coordinate.
 * @return pixel color (RGB).
 */
uint32 gfx_getpix(SURFACE *surface, int x, int y) {
   uint32 *pix = pix_addr(surface, x, y);
   uint8 r, g, b;
   SDL_GetRGB(*pix, surface->image->format, &r, &g, &b);
   return COLOR(r, g, b);
}

/**
 * Initialize a video mode.
 * @param title of the window.
 * @param width of the window.
 * @param height of the window.
 * @return pointer to the initialized surface or 0 if the call failed.
 */
SURFACE *gfx_init(char *title, int width, int height) {
   SURFACE * image = malloc(sizeof(SURFACE));
   image->lock = INIT_SPINLOCK;
   if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Unable to initialize SDL!\n");
      
      return NULL;
   }
   //SURFACE * image = malloc(sizeof(SURFACE));
   image->ren = creer_fenetre(width, height, title, &(image->window));
   
   image->image = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
   image->text_layer = SDL_CreateRGBSurface(0, NB_COL * CHAR_PIX_W, NB_ROW * CHAR_PIX_H, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
   image->lock = INIT_SPINLOCK;
   
   return image;
}

SDL_Renderer * creer_fenetre(int x, int y, char * title, SDL_Window** pWindow){
   *pWindow = SDL_CreateWindow(title, 0, 0, x, y, SDL_WINDOW_SHOWN);
   if(pWindow)
      return SDL_CreateRenderer(*pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   return NULL;
}

/**
 * Check whether the ESC key was pressed or windows close button was clicked.
 * @return true if a key was pressed, false otherwise.
 */
bool gfx_is_esc_pressed() {
   SDL_Event event;
   if (SDL_PollEvent(&event)) {
      // Exit when window's close button is clicked
      if (event.type == SDL_QUIT) {
         return true;
      }
      // Exit when ESC is pressed
      else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
         return true;
      }
   }
   return false;
}

/**
 * Present the surface.
 * @param surface to present.
 */
void gfx_present(SURFACE *surface) {
   SDL_Texture * tex = SDL_CreateTextureFromSurface(surface->ren, surface->image);
   
   SDL_Rect rect_source, rect_dest;
   rect_source.x = rect_source.y = rect_dest.y = rect_dest.x   = 0;
   SDL_GetWindowSize(surface->window, &(rect_source.w), &(rect_source.h));
   rect_dest.h = rect_source.h;
   rect_dest.w = rect_source.w;
   
   SDL_RenderClear(surface->ren);
   SDL_RenderCopy(surface->ren, tex, &rect_source, &rect_dest);
   SDL_DestroyTexture(tex);
   
   // Render layer2 (text)
   
   rect_dest.x = rect_source.w - NB_COL * CHAR_PIX_W;
   rect_dest.y = rect_source.h - NB_ROW * CHAR_PIX_H;
   rect_source.x = rect_source.y = 0;
   rect_source.w = rect_dest.w = NB_COL * CHAR_PIX_W;
   rect_source.h = rect_dest.h = NB_ROW * CHAR_PIX_H;
   tex = SDL_CreateTextureFromSurface(surface->ren, surface->text_layer);
   SDL_RenderCopy(surface->ren, tex, &rect_source, &rect_dest);
   
   // affiche le tout à l'image
   SDL_RenderPresent(surface->ren);
}

bool gfx_lock(SURFACE *surface){
   lock_spin(&(surface->lock));
   return true;
}

void gfx_unlock(SURFACE *surface){
   unlock_spin(&(surface->lock));
   //(*surface)->lock = OS_SPINLOCK_INIT;
}

/**
 * Close the graphic mode.
 */
void gfx_close() {
   SDL_Quit();
}

void * thread_render_present(void * surface){
   SURFACE* s = surface;
   while (1) {
      usleep(40000);  // Check every 0.1 sec. (10 Hz)
      gfx_lock(s);
      gfx_present(s);
      gfx_unlock(s);
   }
   return NULL;
}

void * thread_is_escaped(void * esc_pressed){
   int * esc = (int*) esc_pressed;
   while (!gfx_is_esc_pressed())
      usleep(10000);
   *esc = 1;
   return NULL;
}


void write_char_to_pos(char c, int pos, SURFACE * surface){
   SDL_Surface * font = SDL_LoadBMP("font.bmp");
   
}
