#ifndef TTYIO_H
#define TTYIO_H

#include "typedefs.h"
#include "term.h"
/*
 * This class simulates charater io to terminal
 * Base+0000: status register, read returns  0 if no keys available, 1 if keys available
 * Base+0001: data register, read returns next key from terminal, blocks if not available, write outputs
 */
class ttyio
{
public:
    void set(Word address, Byte value);
    Byte get(Word address);
private:
    Terminal term;
};

#endif // TTYIO_H
