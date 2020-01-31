#define XINERAMA
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
//#include <X11/Xft/Xft.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>

typedef Display display;
typedef Window window;

static int xerror(Display *dpy, XErrorEvent *ee);
static int (*xerrorxlib)(Display*, XErrorEvent*);
static void setup(void);
static void sigchld(int unused);
static void die(const char *fmt, ...);

void die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}

#define Display_val(d) (Data_custom_val(d))
#define Window_val(w) (Data_custom_val(w))

void sigchld(int unused) {
	if (signal(SIGCHLD, sigchld) == SIG_ERR)
		die("can't install SIGCHLD handler:");
	while (0 < waitpid(-1, NULL, WNOHANG));
}

static int xerrorstart(Display *dpy, XErrorEvent *ee) {
    printf("error: Another window manager is already running\n");
    return -1;
}

int xerror(Display *dpy, XErrorEvent *ee) {
	if (ee->error_code == BadWindow
	|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
	|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
	|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
	|| (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
	|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
	|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
		return 0;
	fprintf(stderr, "dwm: fatal error: request code=%d, error code=%d\n",
		ee->request_code, ee->error_code);
	return xerrorxlib(dpy, ee); /* may call exit */
}

value caml_x11_SelectInput(value display, value w, value event_mask) {
    XSelectInput(Display_val(display), Int_val(w), (long)Int_val(event_mask));
}

// string -> display
value caml_x11_OpenDisplay(value v) {
    const char* str = String_val(v);
    return (value)XOpenDisplay(str);
}

// display -> int
value caml_x11_DefaultScreen(value display) {
    return (value)DefaultScreen(Display_val(display));
}

value caml_x11_DisplayWidth(value display, value screen) {
    return (value)DisplayWidth(Display_val(display), Int_val(screen));
}

value caml_x11_DisplayHeight(value display, value screen) {
    return (value)DisplayHeight(Display_val(display), Int_val(screen));
}

// display -> screen -> window
value caml_x11_RootWindow(value display, value screen) {
    return (value)RootWindow(Display_val(display), Int_val(screen));
}

value caml_sigchld(value v) {
    sigchld(Int_val(v));
    return Val_unit;
}

value caml_check_for_other_wm(value dpy) {
    xerrorxlib = XSetErrorHandler(xerrorstart);

    XSelectInput(Display_val(dpy), DefaultRootWindow(Display_val(dpy)), SubstructureRedirectMask);
    XSync(dpy, False);
    XSetErrorHandler(xerror);
    XSync(dpy, False);

    return 0;
}
