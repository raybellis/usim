#ifndef TTYIO_H
#define TTYIO_H

#include "typedefs.h"
#include "term.h"
/*
 * This class simulates charater io to terminal
 * Base+0000: status register, lsbit#0 is read available, lsbit#1 is write available
 * Base+0001: data register, read returns next key from terminal, blocks if not available, write outputs
 */
class ttyio
{
public:
    ttyio(const char *input_device, const char *output_device);
    ttyio(int ifd = 0, int ofd = 1);
    ~ttyio();
    void set(Word address, Byte value);
    Byte get(Word address);
private:
    Terminal *term;
};

#endif // TTYIO_H
