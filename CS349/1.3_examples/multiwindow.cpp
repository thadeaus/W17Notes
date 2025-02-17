/*
CS 349 Code Examples: X Windows and XLib

    multiwindow     Opens and draws into two windows

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o openwindow openwindow.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./openwindow

Note: the -L option and -lstdc++ may not be needed on some machines.

*/

#include <iostream>
#include <list>
#include <cstdlib>
#include <unistd.h>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

const int Border = 5;


/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}


void drawPointsInCorners(XInfo &xinfo) {
	Display  *display = xinfo.display;
	Window   win = xinfo.window;
	GC       gc = xinfo.gc[1];

	XWindowAttributes windowInfo;
	XGetWindowAttributes(display, win, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	/* draw one pixel near each corner of the window */
	XDrawPoint(display, win, gc, 5, 5);
	XDrawPoint(display, win, gc, 5, height-5);
	XDrawPoint(display, win, gc, width-5, 5);
	XDrawPoint(display, win, gc, width-5, height-5);
}

void drawStuff(XInfo &xinfo, int gcIndex, int x, int y) {
	Display  *display = xinfo.display;
	Window   win = xinfo.window;
	GC       gc = xinfo.gc[gcIndex];


	/* draw two intersecting lines, one horizontal and one vertical, */
	/* which intersect at point "x,y".                            */
	XDrawLine(display, win, gc, x, y-30, x, y+200);
	XDrawLine(display, win, gc, x-30, y, x+200, y);

	/* now use the XDrawArc() function to draw a circle whose diameter */
	/* is 30 pixels, and whose center is at location 'x,y'.         */
	XDrawArc(display, win, gc, x-(30/2), y-(30/2), 30, 30, 0, 360*64);

	{
		XPoint points[] = {
				{x+200, y+50},
				{x+250, y+80},
				{x+200, y+80},
				{x+200, y+50}
			};
		int npoints = sizeof(points)/sizeof(XPoint);

		/* draw a small triangle at the top-left corner of the window. */
		/* the triangle is made of a set of consecutive lines, whose   */
		/* end-point pixels are specified in the 'points' array.       */
		XDrawLines(display, win, gc, points, npoints, CoordModeOrigin);
	}

	/* draw a rectangle whose top-left corner is at 'x+120,y+50', its width is */
	/* 50 pixels, and height is 60 pixels.                                  */
	XDrawRectangle(display, win, gc, x+120, y+50, 50, 60);

	/* draw a filled rectangle of the same size as above, to the left of the */
	/* previous rectangle.                                                   */
	XFillRectangle(display, win, gc, x+60, y+50, 50, 60);
}




/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo, Window root, int x, int y, int width, int height) {
	XSizeHints hints;
	unsigned long white, black;

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = x;
	hints.y = y;
	hints.width = width;
	hints.height = height;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow(
		xInfo.display,				// display where window appears
		root, 						// window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour

	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"x2_simpleDrawing",	// window's title
		"SD",				// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/*
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     7, LineSolid, CapRound, JoinMiter);

	i = 2;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     7, LineOnOffDash, CapButt, JoinBevel);


	XSelectInput(xInfo.display, xInfo.window,
		ButtonPressMask | KeyPressMask | ExposureMask);

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );

}

void repaintWindow1(XInfo xinfo) {
	drawPointsInCorners(xinfo);
	drawStuff(xinfo, 0, 30, 50);
	drawStuff(xinfo, 1, 180, 200);
	drawStuff(xinfo, 2, 330, 350);
}


void repaintWindow2(XInfo xinfo) {
	drawStuff(xinfo, 0, 30, 50);
}

/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo1;
	XInfo xInfo2;
	bool isSibling = true;

	if (argc != 2) {
		error("Usage: ./multiwindow [sib|child]");
	} else if (strcmp(argv[1], "sib") == 0) {
		isSibling = true;
	} else if (strcmp(argv[1], "child") == 0) {
		isSibling = false;
	} else {
		error("Usage: ./multiwindow [sib|child]");		
	}


   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/
	Display * display = XOpenDisplay( "" );
	if ( !display )	{
		error( "Can't open display." );
	}

   /*
	* Find out some things about the display you're using.
	*/
	int screen = DefaultScreen( display );
	xInfo1.display = display;
	xInfo1.screen = screen;

	// create window 1
	initX(argc, argv, xInfo1, DefaultRootWindow( display ), 100, 100, 800, 600);

	// create window 2
	xInfo2.display = display;
	xInfo2.screen = screen;

	if (isSibling)
		// make window 2 a sibling of window 1
		initX(argc, argv, xInfo2, DefaultRootWindow( display ), 50, 50, 250, 125);
	else
		// make window 2 a child of window 1
		initX(argc, argv, xInfo2, xInfo1.window, 50, 50, 250, 125);

	XFlush(display);
	sleep(1);	// let server get set up before sending drawing commands

	// need to repaint both windows
	repaintWindow1(xInfo1);
	repaintWindow2(xInfo2);

	XEvent event;

	while( true ) {
		XNextEvent( display, &event );
		switch( event.type ) {
			case ButtonPress:
				if (event.xany.window == xInfo1.window)
					cout << "Got button press in window 1!\n";
				else if (event.xany.window == xInfo2.window)
					cout << "Got button press in window 2!\n";
				break;
			case KeyPress:
				if (event.xany.window == xInfo1.window)
					cout << "Got key press in window 1!\n";
				else if (event.xany.window == xInfo2.window)
					cout << "Got key press in window 2!\n";
				break;
			case Expose:
				// only repaint window which sent the expose
				if (event.xany.window == xInfo1.window)
					repaintWindow1(xInfo1);
				else if (event.xany.window == xInfo2.window)
					repaintWindow2(xInfo2);
				break;
		}
	}

	/* flush all pending requests to the X server. */
	XFlush(display);

	XCloseDisplay(display);
}
