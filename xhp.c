/*
 * xhp -- hide idle X11 pointer.
 */

#include <err.h>
#include <time.h>

#include <X11/Xlib.h>

#define IDLE_NSEC 810000000L

int
main(void)
{
	Display *dpl;
	Window w;
	XEvent ev;
	Pixmap pixmap;
	Cursor cur;
	XColor col;
	struct timespec dly;

	if ((dpl = XOpenDisplay(NULL)) == NULL)
		err(1, "XOpenDisplay()");
	w = DefaultRootWindow(dpl);
	dly.tv_sec = 0;
	dly.tv_nsec = IDLE_NSEC;
	col = (XColor){0};
	pixmap = XCreatePixmap(dpl, w, 1, 1, 1);
	cur = XCreatePixmapCursor(dpl, pixmap, pixmap, &col, &col, 0, 0);
	XFreePixmap(dpl, pixmap);
	for (;;) {
		if (XGrabPointer(dpl, w, 0, PointerMotionMask | ButtonPressMask,
		    GrabModeSync, GrabModeAsync, w, cur, CurrentTime) !=
		    GrabSuccess)
			continue;
		XAllowEvents(dpl, SyncPointer, CurrentTime);
		do {
			XMaskEvent(dpl, PointerMotionMask |
			    ButtonPressMask, &ev);
			XAllowEvents(dpl, ReplayPointer, CurrentTime);
			XUngrabPointer(dpl, CurrentTime);
		} while(XPending(dpl));
		nanosleep(&dly, 0);
	}
	return (0);
}
