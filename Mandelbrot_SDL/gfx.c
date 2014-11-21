/**
 * @file gfx.c
 * @brief Minimalistic SDL wrapper providing pixel drawing functions.
 * @author Florent Gluck, florent.gluck@hesge.ch
 * @date October, 2014
 * @version 0.1
 */

#include "gfx.h"
#include <string.h>

#define NB_ROW 2
#define NB_COL 20
#define CHAR_PIX_W 14
#define CHAR_PIX_H 18
#define ZOOM 1
#define F_PADDING 14
#define F_PADD_X 2


int WIDTH = 1920;
int HEIGHT = 1080;
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
 *
SURFACE *gfx_init(char *title, int width, int height) {
   SURFACE * image = malloc(sizeof(SURFACE));
   if(!image)
   {
      fprintf(stderr,"error allocating SRUFACE memory\n");
      exit(1);
   }
   image->lock = INIT_SPINLOCK(&(image->lock),0);
   if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Unable to initialize SDL!\n");
      
      return NULL;
   }
   image->ren = creer_fenetre(width, height, title, &(image->window));
   image->string = malloc(sizeof(char));
   if(!image->string)
   {
      fprintf(stderr,"error allocating string memory for status\n");
      exit(1);
   }
   for (int i = 0; i < NB_COL * NB_ROW ; i++)
      image->string[i] = '\0';
   image->image = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
   SDL_Surface * font = SDL_LoadBMP("font.bmp");
   if (font == NULL)
      printf("Warning, font.bmp hasn't been found!\n");
   image->text_layer = SDL_CreateTextureFromSurface(image->ren, font);
   SDL_FreeSurface(font);
   image->lock = INIT_SPINLOCK(0,0);
   
   return image;
}*/

SURFACE *gfx_init(char *title, int width, int height) {
   if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Unable to initialize SDL!\n");
      return NULL;
   }
   SURFACE * image = malloc(sizeof(SURFACE));
   // HACK: software surfaces shouldn't require locking
   image->image = SDL_SetVideoMode(width, height, DEPTH, SDL_SWSURFACE);
   image->font = SDL_LoadBMP("font.bmp");
   image->lock = INIT_SPINLOCK(&(image->lock),0);
   
   if (image->image == NULL) {
      printf("Unable to initialize SDL video mode!\n");
      SDL_Quit();
      return NULL;
   }
   
   image->string = malloc(sizeof(char)* NB_ROW * NB_COL);
   if(!image->string)
   {
      fprintf(stderr,"error allocating string memory for status\n");
      exit(1);
   }
   for (int i = 0; i < NB_COL * NB_ROW ; i++)
      image->string[i] = '\0';
   
   
   SDL_WM_SetCaption(title, 0);
   
   return image;
}
/*
SDL_Renderer * creer_fenetre(int x, int y, char * title, SDL_Window** pWindow){
   *pWindow = SDL_CreateWindow(title, 0, 0, x, y, SDL_WINDOW_SHOWN);
   if(pWindow)
      return SDL_CreateRenderer(*pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   return NULL;
}*/

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
   int pos = 0;
   while (surface->string[pos] != '\0'){
      write_char_to_pos(surface->string[pos], pos, surface);
      pos++;
   }
   SDL_Flip(surface->image);
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
void gfx_close(SURFACE * surface) {
   SDL_Quit();
   //free(surface);
}
/**
 * The gfx_present function but designed to be threaded
 * @param surface The surface to be presented
 */
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
      usleep(100000);
   return NULL;
}

//Write a char on the top left of the screen. font.bmp is model. "pos" goes from 0 to 40.
void write_char_to_pos(char c, int pos, SURFACE * surface){
   SDL_Rect rect_source, rect_dest;
   if (c >= 'A' && c <='Z') {
      rect_source.y = F_PADDING;
      rect_source.x =  (c - 'A') * CHAR_PIX_W + (c - 'A') * F_PADD_X;
   }
   else if (c >= 'a' && c <='z') {
      rect_source.y = (2 * F_PADDING) + CHAR_PIX_H;
      rect_source.x =  (c - 'a') * CHAR_PIX_W + (c - 'a') * F_PADD_X;
   }
   else if (c >= '0' && c <='9') {
      rect_source.y = (3 * F_PADDING) + 2 * CHAR_PIX_H;
      rect_source.x =  (c - '0') * CHAR_PIX_W + (c - '0') * F_PADD_X;
   }
   else if (c == '%') {
      rect_source.y = (4 * F_PADDING) + 3 * CHAR_PIX_H;
      rect_source.x =  4 * CHAR_PIX_W + 4 * F_PADD_X;
   }
   else if (c == '.') {
      rect_source.y = (3 * F_PADDING) + 2 * CHAR_PIX_H;
      rect_source.x =  10 * CHAR_PIX_W + 10 * F_PADD_X;
   }
   else{
      rect_source.y = F_PADDING;
      rect_source.x =  ('Z' - 'A' + 1) * CHAR_PIX_W + ('Z' - 'A' + 1) * F_PADD_X;
   }
   
   rect_dest.x = (((pos % NB_COL) * CHAR_PIX_W) % (NB_COL * CHAR_PIX_W)) * ZOOM;
   rect_dest.y = (pos / NB_COL) * CHAR_PIX_H * ZOOM;
   
   rect_source.w = CHAR_PIX_W;
   rect_dest.w = CHAR_PIX_W * ZOOM;
   rect_source.h = CHAR_PIX_H;
   rect_dest.h = CHAR_PIX_H * ZOOM;
   SDL_BlitSurface(surface->font, &rect_source, surface->image, &rect_dest);
}


void gfx_print(char * string, SURFACE * surface){
   strcpy(surface->string, string);
}
