INTRODUCTION:
  PMStars is a PM program that I wrote to learn about the OS/2 GPI
and the PM timer function.  I thought I'd start with something simple,
but kinda fun -- a simple starfield came mind.
  PMStars is complete as-is, but could do with some programming
polish.  The source code is included (see below).
  Best of all:  PMStars is FREE !

INSTALLATION:
  Copy the *.exe files to a directory of your choice.  If you want to
view/edit the source code, copy the PMStars.c and PMStars.h files
as well.

RUNNING THE PROGRAM:
  Double click the PMStars.exe (or PMStarsd.exe) file to start:

  PMStars will display the starfield with either a single 'dot' or
line for each star.  The line length depends on the stars speed.
    -> Use LMB (left mouse button) to set the star's origin,
       RMB (right mouse button) to switch between 'dot' and line.
    Resize the window for some interesting (and intentional) effects !

  PMStarsd will display the starfield with either a line or a star
number.
    -> LMB will set the star's origin (and marker, if applic),
       RMB will switch between line and star numbers.
    This program varient was used during debugging, and I thought it
      might be helpful to include it in the distribution.

ABOUT THE SOURCE:
  I'm including the source because it's sooo simple but I couldn't find
similar source in the usual places.
  The code is documented well enough to (hopefully) make the source
self-explanatory -- assuming a basic knowledge of c, PM and GPI.
Much improvement is possible, and desired !
  Both executables are produced from the same source; the DEBUG variable
(see PMStars.h) was defined for PMStarsd, and undefined for PMStars.
  I used the WATCOM 10.6 compiler to generate this program.

Don Moore
moored@iname.com
12 Aug 99
