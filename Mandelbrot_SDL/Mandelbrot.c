//
//  Mandelbrot.c
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 06/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#include <stdio.h>
#include "Mandelbrot.h"
#include "Pile.h"

void *thread_mandelbrot(void *arg)
{
   Pile *p = (Pile*) arg;
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
      
   }
   
   return NULL;
}
