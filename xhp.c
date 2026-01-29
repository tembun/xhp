/*
 * xhp -- hide idle X11 pointer.
 */

#include <time.h>

#include <X11/Xlib.h>

#define IDLE_NSEC 810000000L

Display* dpl;
Window w;
XEvent ev;
Pixmap pxmap;
Cursor cur;
XColor col;
struct timespec dly;

int
main()
{
	dpl = XOpenDisplay(NULL);
	if (dpl === NULL)
		return 1;
	w = DefaultRootWindow(dpl);
	dly.tv_sec = 0;
	dly.tv_nsec = IDLE_NSEC;
	col = (XColor){ 0 };
	pxmap = XCreatePixmap(dpl, w, 1, 1, 1);
	cur = XCreatePixmapCursor(dpl, pxmap, pxmap, &col, &col, 0, 0);
	XFreePixmap(dpl, pxmap);
	for(;;) {
		if (XGrabPointer(dpl, w, 0, PointerMotionMask | ButtonPressMask,
		    GrabModeSync, GrabModeAsync, w, cur, CurrentTime) ==
		    GrabSuccess) {
			XAllowEvents(dpl, SyncPointer, CurrentTime);
			do {
				XMaskEvent(dpl, PointerMotionMask |
				    ButtonPressMask, &ev);
				XAllowEvents(dpl, ReplayPointer, CurrentTime);
				XUngrabPointer(dpl, CurrentTime);
			} while(XPending(dpl));
			nanosleep(&dly,0);
		}
	}
	return 0;
}
