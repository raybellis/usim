#ifndef DISKIO_H
#define DISKIO_H

#include "typedefs.h"
/*
 * This class simulates disk io device
 * Registers:
 * Base+0000: block number - write resets internal buffer pointer
 * Base+0002: disk number - write resets internal buffer pointer
 * Base+0003: data register - 1K, 1024 writes write, first read after reset reads
buffer
 */
class diskio
{
public:
    diskio();
    void set(Word address, Byte value);
    Byte get(Word address);
private:
    void setWord(Byte offset, Word *target, Byte value);
    void read();
    void write();
    int position();
    Word block_number;
    Byte disk_number;
    int transfer_buffer_index;
    Byte transfer_buffer[1024];
    int fd[4]; // Maximum of four disks
};

#endif // DISKIO_H
