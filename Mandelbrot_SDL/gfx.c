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

int initiated = 0;
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
   image->lock = OS_SPINLOCK_INIT;
   if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Unable to initialize SDL!\n");
      
      return NULL;
   }

   image->window = SDL_CreateWindow(title, 0, 0, width, height, SDL_WINDOW_SHOWN);
   
   image->image = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
   image->ecran = SDL_GetWindowSurface(image->window);
   image->text_layer = SDL_CreateRGBSurface(0, NB_COL * CHAR_PIX_W, NB_ROW * CHAR_PIX_H, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
   
   return image;
}

void creer_fenetre(int x, int y, char * title, SDL_Window** pWindow){
   *pWindow = SDL_CreateWindow(title, 0, 0, x, y, SDL_WINDOW_SHOWN);
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

   
   SDL_BlitSurface(surface->image, NULL, surface->ecran, NULL);
   
   SDL_UpdateWindowSurface(surface->window);
   
   // Render layer2 (text)
   
   
   // affiche le tout à l'image
   //SDL_RenderPresent(surface->ren);
}

bool gfx_lock(SURFACE *surface){
   SDL_LockSurface(surface->image);
   return true;
}

void gfx_unlock(SURFACE *surface){
   SDL_UnlockSurface(surface->image);
}

/**
 * Close the graphic mode.
 */
void gfx_close() {
   SDL_Quit();
}

void * thread_render_present(void * surface){
   SURFACE* s = (SURFACE*)surface;
   //*s = gfx_init("MandelBrot", WIDTH, HEIGHT);
   initiated = 1;
   while (1) {
      usleep(40000);  // Check every 0.1 sec. (10 Hz)
      gfx_lock(s);
      gfx_present(s);
      gfx_unlock(s);
   }
   return NULL;
}

void write_char_to_pos(char c, int pos, SURFACE * surface){
   SDL_Surface * font = SDL_LoadBMP("font.bmp");
   
}
