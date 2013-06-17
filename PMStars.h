#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INCL_GPI
#define INCL_WIN
#include <os2.h>

#ifndef DEBUG
#define DEBUG
#undef DEBUG
#endif

#define LAYER 3
#define SPEED 10
#ifdef DEBUG
#define STARS 15
#else
#define STARS 50
#endif
#define SCREEN_WIDTH 500L
#define SCREEN_HEIGHT 400L
#define PI 3.14159265359
#define SAVE_NAME  "WINSAVE"
#define SAVE_KEY "WINDOWPOS"
#define TIMER_FIX_EXIT 0

#define MY_TIMER TID_USERMAX-1
#define WM_UUNSET WM_USER+1
#define WM_USET WM_USER+2

MRESULT EXPENTRY ClientWndProc(HWND,USHORT,MPARAM,MPARAM);


