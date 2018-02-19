//
//
//	mc6809_X.h
//
//	(C) R.P.Bellis 1994
//

#ifndef __mc6809_X__h
#define __mc6809_X__h

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "mc6809.h"

class mc6809_X : virtual public mc6809 {

// X11 stuff
protected:

		Display			*dpy;
		Screen			*scrn;
		Window			 win;
		Pixmap			 pix;
		GC			 gc, inv_gc;
		XFontStruct		*fs;
		int			 ww, wh;

// X11 text stuff
		int			 cw, ch;
		void			 text(int x, int y, const char *str,
						int rev = 0);

// Processor status functions
public:

	virtual void			 status();

// Public constructor and destructor
public:

					 mc6809_X();
					~mc6809_X();

};

#endif // __mc6809_X__h
