//
//  spinlock.h
//  Mandelbrot_SDL
//
//  Created by Alexis Marquet on 12/11/14.
//  Copyright (c) 2014 Olivier. All rights reserved.
//

#ifndef __Mandelbrot_SDL__spinlock__
#define __Mandelbrot_SDL__spinlock__

#define __USE_XOPEN2K
#include <pthread.h>
#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#define INIT_SPINLOCK(x,y) OS_SPINLOCK_INIT
#define SPINLOCK_T OSSpinLock
#define lock_spin(x) OSSpinLockLock(x)
#define unlock_spin(x) OSSpinLockUnlock(x)
#define trylock_spin(x) OSSpinLockTry(x)
#else
//#include <linux/spinlock_types.h>
#include <pthread.h>
#define INIT_SPINLOCK(x,y) PTHREAD_PROCESS_SHARED//pthread_spin_init(x,y)
#define SPINLOCK_T pthread_spinlock_t
#define lock_spin(x) pthread_spin_lock(x)
#define unlock_spin(x) pthread_spin_unlock(x)
#define trylock_spin(x) pthread_spin_trylock(x)
#endif


#endif /* defined(__Mandelbrot_SDL__spinlock__) */
