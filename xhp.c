/*
 * xhp -- hide idle X11 pointer.
 */

#include <err.h>
#include <stdbool.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define IDLE_NSEC 810000000L
#define TOGGLE_KEY XK_Delete
#define TOGGLE_KEY_MODIFIER ControlMask

int
main(void)
{
	Display *dpl;
	Window w;
	XEvent ev;
	Pixmap pixmap;
	Cursor cur;
	XColor col;
	KeyCode toggle_kc;
	bool need_hide;
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
	toggle_kc = XKeysymToKeycode(dpl, TOGGLE_KEY);
	XGrabKey(dpl, toggle_kc, TOGGLE_KEY_MODIFIER, w, False, GrabModeAsync,
	    GrabModeAsync);
	XSync(dpl, False);
	need_hide = true;
	for (;;) {
		if (need_hide)
hide:
			if (XGrabPointer(dpl, w, 0, PointerMotionMask | ButtonPressMask,
			    GrabModeSync, GrabModeAsync, w, cur, CurrentTime) !=
			    GrabSuccess)
				continue;
		XAllowEvents(dpl, SyncPointer, CurrentTime);
		do {
			XMaskEvent(dpl, PointerMotionMask |
			    ButtonPressMask | KeyPressMask | KeyReleaseMask, &ev);
			switch (ev.type) {
			case KeyPress:
				if ((need_hide = !need_hide))
					goto hide;
				break;
			case KeyRelease:
				continue;
			default:
				break;
			}
			XAllowEvents(dpl, ReplayPointer, CurrentTime);
			XUngrabPointer(dpl, CurrentTime);
		} while(XPending(dpl));
		nanosleep(&dly, 0);
	}
	return (0);
}
