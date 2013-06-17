#include "pmstars.h"

HAB hab;
int track=0;  /* Star will 'track' */
LONG back_clr=CLR_BLACK;  /* (client) Window background color */

void main(int argc, char *argv[]) {
  HMQ hmq;
  QMSG qmsg;
  HWND hwndFrame, hwndDrawing;
  ULONG flFrameFlags= FCF_TITLEBAR | FCF_SYSMENU | FCF_MAXBUTTON |
                      FCF_SIZEBORDER | FCF_TASKLIST;
  char szClientClass[]="PMStars";

/* Standard PM window creation stuff */
  hab=WinInitialize( 0 );
  hmq=WinCreateMsgQueue( hab, 0 );
  WinRegisterClass( hab, szClientClass, (PFNWP)ClientWndProc,
           CS_SIZEREDRAW, (ULONG)0 );
  hwndFrame=WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
                 &flFrameFlags, szClientClass, szClientClass,
                 0L, NULLHANDLE, 0L, &hwndDrawing );
/* Check that window was created then set window location on the screen and display */
  if ( hwndFrame != NULLHANDLE )
    WinSetWindowPos ( hwndFrame, NULLHANDLE, 10L, 10L, SCREEN_WIDTH, SCREEN_HEIGHT,
                      SWP_ACTIVATE | SWP_MOVE | SWP_SIZE | SWP_SHOW ) ;

/* Window message handling loop */
  while( WinGetMsg( hab,&qmsg,NULLHANDLE,0L,0L ) )
    WinDispatchMsg( hab,&qmsg );

/* Application has been terminated (close, etc), so clean-up and exit */
  WinDestroyWindow( hwndFrame );
  WinDestroyMsgQueue( hmq );
  WinTerminate( hab );
  return;
}

MRESULT EXPENTRY ClientWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) {
  static LONG width, height, /* (client) Window width (x-coord) and height (y-coord) */
              center_x, center_y;  /* Star origin */
  static HDC hdc;  /* PM device context (GPI Presentation Space needs this) */
  static HPS hps;  /* GPI Presentation Space; graphics calls draw into this */
  static POINTL star[STARS*2];  /* Star array:  even indexes are previous coords,
                                                odd indexes are current coords;
                                                0 is an even index
                                */
  static int index,  /* Loop variable indicating an index into the star array */
             star_speed[STARS*2],  /* Star 'speed', an offset from a star's current pos'n:
                                      even indexes are for x-coords,
                                      odd indexes are for y-coords;
                                      0 is an even index.
                                      Ranges from 1 to SPEED
                                     */
             signum[STARS*2];  /* Star 'direction':  even indexes are for x-coords,
                                                     odd indexes are for y-coords;
                                                     0 is an even index.
                                                     value is either (+1) or (-1)
                               */
  static ULONG my_timer;  /* System (PM) number indicating unique (system) timer number
                             for this app
                          */
#ifdef DEBUG
  static LONG my_marker;  /* Marker type (defined by PM) */
#endif

  switch(msg) {
  case WM_CREATE: {  /* App has just started, so setup any data needed to continue */
    SIZEL size;  /* (PM) struct indicating (client) window size
                    (contains both x- and y-coords)
                 */
    int rn;

    srand( (unsigned int)time(0) );
    width=size.cx=SCREEN_WIDTH; center_x=(SHORT)(width/2);
    height=size.cy=SCREEN_HEIGHT; center_y=(SHORT)(height/2);
  /* Open Window Device Context (screen), create GPI Presentation Space
     in that device context, and tell PM to only draw elements to the
     GPI Presentation Space created
  */
    hdc=WinOpenWindowDC( hwnd );
    hps=GpiCreatePS(hab, hdc, &size,
                    PU_PELS | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC);
    GpiSetDrawingMode( hps, DM_DRAW );
  /* Initialize all stars (current and previous), their x,y speeds and x,y directions */
    for( index=0; index<STARS*2; index+=2) {  /* Spin through all the previous stars */
      star[index].x=0L; star[index].y=0L;
      star[index+1].x=center_x; star[index+1].y=center_y;
      rn=(rand() % SPEED) +1; star_speed[index] = rn;
      rn=(rand() % SPEED) +1; star_speed[index+1] = rn;
      signum[index]=( (rand() % 2) ? 1 : -1 );
      signum[index+1]=( (rand() % 2) ? 1 : -1 );
    } /* endfor index */
#ifdef DEBUG
  /* Define the display marker symbol (small circle, others are available) */
    my_marker=MARKSYM_SMALLCIRCLE;
    GpiSetMarker( hps, my_marker );
#endif
  /* Start the animation timer -- note hab is global, 50 mS timeout */
    my_timer=WinStartTimer( hab, hwnd, 0L, (ULONG)50 );
    break; }  /* endcase WM_CREATE */

  case WM_USET: {  /* Message processed when stars must be set (shown) in window */
    LONG nc; int rn;
    for( index=1; index<STARS*2; index+=2) {  /* Spin through the current loc'ns */
  /* Select a color for the (new) star */
      rn=rand() % 8;
      switch ( rn ) {
        case 0: nc=CLR_WHITE; break;
        case 1: nc=CLR_BLUE; break;
        case 2: nc=CLR_RED; break;
        case 3: nc=CLR_GREEN; break;
        case 4: nc=CLR_YELLOW; break;
        case 5: nc=CLR_CYAN; break;
        case 6: nc=CLR_PINK; break;
        case 7: nc=CLR_NEUTRAL; break;
        default: nc=CLR_WHITE;
      }  /* endswitch rand() */
      GpiSetColor( hps, nc );
  /* 'Track' processing: */
      if(track)  /* Stars should not have a 'track' */
#ifdef DEBUG
      {
      /* Marker processing:  Produce an ASCII string for the star number,
         then draw the string and marker at the new point
      */
        char outstr[10]; long ret;
        sprintf( outstr, "%d", index/2+1);
        ret=GpiCharStringAt( hps, &star[index-1], strlen(outstr), outstr );
        GpiMarker( hps, &star[index-1] );
      }
#else
        GpiSetPel( hps, &star[index] );  /* Draw the new point */
#endif
      else {  /* Stars should have a 'track' */
      /* Get the last point drawn then draw a line to the new point */
        GpiMove( hps, &star[index-1] );
        GpiLine( hps, &star[index] );
      }  /* endif track */
    }  /* endfor index */
    break; }  /* endcase WM_USET */

  case WM_UUNSET:  /* Message processed when stars must be unset (erased) in window */
  /* Draw color is now the background color (thereby 'erasing' the point) */
    GpiSetColor( hps, back_clr );
  /* 'Track' processing: */
    if(track)  /* Stars should not have a 'track' */
      for( index=0; index<STARS*2; index+=2)  /* Spin through all the previous stars... */
#ifdef DEBUG
    {
    /* Marker processing:  Produce an ASCII string for the star number,
       then (un)draw the string and marker at the new point
    */
      char outstr[10]; long ret;
      sprintf( outstr, "%d", index/2+1);
      ret=GpiCharStringAt( hps, &star[index], strlen(outstr), outstr );
      GpiMarker( hps, &star[index] );
    }
#else
    GpiSetPel( hps, &star[index+1] );  /* Undraw the point
                                          (note current point, not previous one !)
                                       */
#endif
    else  /* Stars should have a 'track' */
    /* Get the last point drawn then undraw the line ('track') */
      for( index=0; index<STARS*2; index+=2) {
        GpiMove( hps, &star[index] );
        GpiLine( hps, &star[index+1] );
      }  /* endfor index */
    break;  /* endcase WM_UUNSET */

  case WM_PAINT: { /* Fill a neutral (black) background in the (client) window */
    RECTL rcl;
    HPS hpsX;
    hpsX=WinBeginPaint( hwnd, NULLHANDLE, &rcl );
    WinFillRect( hps, &rcl, back_clr );
    WinEndPaint( hpsX );
    WinSendMsg( hwnd, WM_UUNSET, 0L, 0L );
    WinSendMsg( hwnd, WM_USET, 0L, 0L );
    break;
  }  /* endcase WM_PAINT */

  case WM_CLOSE:  /* Message processed when a request to close the application was received */
  /* Delete the current GPI Presentation Space, then signal app termination
     to the caller (main)
  */
    GpiDestroyPS(hps);
    WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
    break;

  case WM_SIZE: {  /* Message processed when user has resized the (client) window */
  /* Compute new size from input params and undraw old points & 'tracks' */
    width=SHORT1FROMMP(mp2); height=SHORT2FROMMP(mp2);
    WinSendMsg( hwnd, WM_UUNSET, 0L, 0L );
  /* Calc star loc'ns from the new origin (if req'd):
       - previous x-coord is calc'd if new window size is smaller than previous (x-coord)
       - previous y-coord is calc'd if new window size is smaller than previous (y-coord)
     Note:  x- and y-coords are recalculated are independantly ! (a Feature)
            this method creates the 'runner' lines when 'tracking' (also a Feature)
  */
    for( index=1; index<STARS*2; index+=2) {
      if(center_x > width ) star[index].x=width/2;
      if(center_y > height) star[index].y=height/2;
    }  /* endfor index */
  /* Calc new x,y origins if req'd, then draw all the points / 'tracks' */
    if(center_x > width) center_x=width/2;
    if(center_y > height) center_y=height/2;
    WinPostMsg( hwnd, WM_USET, 0L, 0L);
    break;
  }  /* endcase WM_SIZE */

  case WM_BUTTON2DOWN:  /* Message processed when RMB (mouse button 2) is pressed:
                           RMB means toggle from line ('track') to marker (if
                           DEBUG is defined), or line to point (if DEBUG is
                           not defined)
                        */
  /* Erase current or previous points / 'tracks', toggle track variable to
     signal RMB-requested change, then redraw the points / 'tracks'
  */
    WinSendMsg( hwnd, WM_UUNSET, 0L, 0L);
    track=(track ? 0 : 1);
    WinPostMsg( hwnd, WM_USET, 0L, 0L);
    break;  /* endcase WM_BUTTON1DBLCLK */

  case WM_BUTTON1DOWN:  /* Message processed when LMB (mouse button 1) is pressed:
                           LMB means redefine a new star origin from the current
                           mouse pos'n (found in input params) and toggle the
                           marker (if DEBUG is defined)
                        */
  /* Change origin to current mouse coords then undraw old points / 'tracks' */
    center_x=SHORT1FROMMP(mp1); center_y=SHORT2FROMMP(mp1);
    WinSendMsg( hwnd, WM_UUNSET, 0L, 0L );
#ifdef DEBUG
  /* Toggle the marker */
    my_marker=( my_marker==MARKSYM_SMALLCIRCLE ? MARKSYM_CROSS : MARKSYM_SMALLCIRCLE );
    GpiSetMarker( hps, my_marker );
#endif
  /* Calc star loc'ns for the new origin:
       - previous x-coord is current loc'n, current x-coord is new origin x-coord
       - previous y-coord is current loc'n, current y-coord is new origin y-coord
     Note:  this method creates the 'runner' lines when 'tracking' (also a Feature)
  */
    for( index=1; index<STARS*2; index+=2) {
      star[index-1].x=star[index].x; star[index].x=center_x;
      star[index-1].y=star[index].y; star[index].y=center_y;
    }  /* endfor index */
    WinPostMsg( hwnd, WM_USET, 0L, 0L);  /* Draw the points / 'tracks' */
    break;  /* endcase WM_BUTTON1DOWN */

  case WM_TIMER: {  /* Message processed when timer has 'timed-out' (i.e., expired),
                       so its time to move the stars !
                    */
    int rn;
  /* Undraw old points / 'track' (Note: this MUST be synchronous, i.e. Send is req'd) */
    WinSendMsg( hwnd, WM_UUNSET, 0L, 0L);
  /* Move the current star points to the previous star points array loc'ns */
    for( index = 0; index < STARS*2; index+=2 ) {
      star[index].x=star[index+1].x; star[index].y=star[index+1].y;
    }  /* endfor index */
  /* Calc current (new) point loc'ns using each star's SPEED */
    for ( index = 1 ; index < STARS*2 ; index+=2) {
    /* Check if this star is out-of-bounds: */
      if ( star[index].x < 0L || star[index].y < 0L  ||
           star[index].x > width || star[index].y > height ) {
      /* Star is out-of-bounds: define current and previous star loc'n as
         current origin (long cast is req'd by PM for Pel loc'ns), then define
         new (star) speed and (star) direction.
      */
        star[index].x = star[index-1].x = (long)center_x;
        star[index].y = star[index-1].y = (long)center_y;
        rn=(rand() % SPEED) +1; star_speed[index] = rn;
        rn=(rand() % SPEED) +1; star_speed[index-1] = rn;
        signum[index]=( (rand() % 2) ? 1 : -1 );
        signum[index-1]=( (rand() % 2) ? 1 : -1 );
      } else {  /* Star is not out-of-bounds, so calc new loc'n from current */
        star[index].x += (signum[index-1] * (long) star_speed[index-1]);
        star[index].y += (signum[index] * (long) star_speed[index]);
      }  /* endif star[index].x */
    } /* End for index */
    WinSendMsg( hwnd, WM_USET, 0L, 0L );  /* Draw the points / 'tracks' */
    return MRFROMSHORT( TRUE ); }  /* endcase WM_TIMER */
  default:
    return WinDefWindowProc(hwnd,msg,mp1,mp2);  /* Message not processed here,
                                                   so process normally
                                                */
  }  /* endswitch msg */
  return MRFROMSHORT( FALSE );
}

