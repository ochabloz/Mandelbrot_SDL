//
//  spinlock.h
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 12/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef __Mandelbrot_SDL__spinlock__
#define __Mandelbrot_SDL__spinlock__

#include <pthread.h>
#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#define SPINLOCK_T OSSpinLock
#define lock_spin(x) OSSpinLockLock(x)
#define unlock_spin(x) OSSpinLockUnlock(x)
#define trylock_spin(x) OSSpinLockTry(x)
#else
#define SPINLOCK_T pthread_spinlock_t
#endif


#endif /* defined(__Mandelbrot_SDL__spinlock__) */
