#ifndef SIDFORTH_H
#define SIDFORTH_H

#include "diskio.h"
#include "ttyio.h"

#ifdef DEBUG
#include "mc6809_debug.h"
# define INHERIT mc6809_debug
#else
#include "mc6809_X.h"
# define INHERIT mc6809_X
#endif
/*
 * virtual sidforth device
 */
class sidforth : public INHERIT
{
public:
    sidforth();
    virtual ~sidforth();
protected:
    virtual Byte			 read(Word);
    virtual void			 write(Word, Byte);
private:
    int xterm(); // Return rd/wr fd of slave pty
    static const Word disk_address = 0xFC00;
    static const Word console_address =  disk_address + 0x10;
    static const Word uart_address = console_address + 0x10;
    diskio disk;
    ttyio *console;
    ttyio *uart;
    FILE *xterm_pipe;
};

#endif // SIDFORTH_H
