//
//	mc6809_X.cc
//
//	(C) R.P.Bellis 1994
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mc6809_X.h"

mc6809_X::mc6809_X()
{
	// Open display and get some neccesary variables
	dpy = XOpenDisplay(0);
	scrn = DefaultScreenOfDisplay(dpy);

	// Get a fixed font
	char				**ret;
	int				count;

	ret = XListFonts(dpy, "*-courier-medium-r-*-14-*", 1, &count);
	if (ret) {
		fs = XLoadQueryFont(dpy, ret[0]);
		if (!fs) {
			fs = XLoadQueryFont(dpy, "fixed");
		}
		XFreeFontNames(ret);
	} else {
		fs = XLoadQueryFont(dpy, "fixed");
	}
	cw = XTextWidth(fs, " ", 1);
	ch = (fs->ascent + fs->descent);

	// Setup default window attributes
	XSetWindowAttributes		attrs;
	long				attr_mask = 0;

	attrs.backing_store = Always;
	attrs.background_pixel = BlackPixelOfScreen(scrn);
	attrs.border_pixel = WhitePixelOfScreen(scrn);
	attrs.event_mask = StructureNotifyMask;
	attr_mask = CWBackingStore | CWEventMask;
	attr_mask |= CWBackPixel | CWBorderPixel;

	// Create the window
	ww = cw * 38 + 8;
	wh = ch * 13 + 8;
	win = XCreateWindow(dpy, RootWindowOfScreen(scrn),
			0, 0, ww, wh, 2, CopyFromParent,
			CopyFromParent, CopyFromParent,
			attr_mask, &attrs);

	// Create a pixmap
	pix = XCreatePixmap(dpy, RootWindowOfScreen(scrn),
		ww, wh, DefaultDepthOfScreen(scrn));

	// Create a GC to go with it
	XGCValues		gcv;
	long			gcv_mask;
	gcv.foreground = WhitePixelOfScreen(scrn);
	gcv.background = BlackPixelOfScreen(scrn);
	gcv.font = fs->fid;
	gcv_mask = GCForeground | GCBackground | GCFont;
	gc = XCreateGC(dpy, pix, gcv_mask, &gcv);

	// And an inverted GC
	gcv.foreground = BlackPixelOfScreen(scrn);
	gcv.background = WhitePixelOfScreen(scrn);
	inv_gc = XCreateGC(dpy, pix, gcv_mask, &gcv);

	// And clear the pixmap
	XFillRectangle(dpy, pix, inv_gc, 0, 0, ww, wh);

	// Set window size
	XSizeHints			sh;
	sh.width = sh.max_width = ww;
	sh.height = sh.max_height = wh;
	sh.flags = PSize | PMaxSize;
	XSetStandardProperties(dpy, win, "mc6809 simulator", "mc6809",
		None, NULL, 0, &sh);

	// Set window class
	XClassHint			xch;
	xch.res_name = strdup("mc6809");
	xch.res_class = strdup("MC6809");
	XSetClassHint(dpy, win, &xch);

	// Put the window on the screen
	XMapWindow(dpy, win);
	XFlush(dpy);

	text( 0, 0, "  PC:");
	text( 0, 1, "   S:");
	text( 0, 2, "   U:");
	text( 0, 4, "   X:");
	text( 0, 5, "   Y:");

	text(15, 1, "EFHINZVC");
	text(10, 2, "  CC:");
	text(10, 4, "   A:");
	text(10, 5, "   B:");

	for (int i = 0; i < 6; ++i) {
		text(4, i + 7, ":");
	}
}

mc6809_X::~mc6809_X()
{
	XDestroyWindow(dpy, win);
}

void mc6809_X::text(int x, int y, const char *str, int rev)
{
	x = x * cw + 4;
	y = (y + 1) * ch + 4 - fs->descent;

	XDrawImageString(dpy, pix, rev ? inv_gc : gc, x, y, str, strlen(str));
}

void mc6809_X::status(void)
{
	static char	tmp[40];

	text( 6, 0, hexstr(pc));
	text( 6, 1, hexstr(s));
	text( 6, 2, hexstr(u));
	text( 6, 4, hexstr(x));
	text( 6, 5, hexstr(y));
	text(15, 4, hexstr(a));
	text(15, 5, hexstr(b));
	text(15, 2, binstr(cc.all));

	Word		stk = ((s >> 3) << 3) - 16;
	Word		stk_tmp = stk;

	for (int i = 0; i < 6; ++i) {
		text(0, i + 7, hexstr(stk));

		tmp[0] = '\0';
		for (int j = 0; j < 8; ++j) {
			Byte		ch = memory[stk + j];
			strcat(tmp, hexstr(ch));
			strcat(tmp, " ");
		}

		for (int j = 0; j < 8; ++j) {
			Byte		ch = memory[stk + j];
			strcat(tmp, ascchr(ch));
		}
		text(6, i + 7, tmp);
		stk+= 8;
	}

	{
		int	x = (s - stk_tmp) % 8;
		int	y = (s - stk_tmp) / 8;
		text(6 + 3 * x, y + 7, hexstr(memory[s]), 1);
		text(30 + x, y + 7, ascchr(memory[s]), 1);
	}

	XCopyArea(dpy, pix, win, gc, 0, 0, ww, wh, 0, 0);
	XFlush(dpy);
}
