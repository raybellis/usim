#include "diskio.h"
#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>

diskio::diskio() :
    block_number(),
    disk_number(),
    transfer_buffer_index(0),
    transfer_buffer(),
    fd()
{
}

void diskio::set(Word address, Byte value)
{
//    fprintf(stderr, "DISKIO: %1X<-%02X\r\n", address, value);
    switch(address) {
    case 0:
    case 1:
        setWord((Byte)address, &block_number, value);
        transfer_buffer_index = 0;
        break;
    case 2:
        disk_number = value & 3; // Assure less than 4
        transfer_buffer_index = 0;
        break;
    case 3:
        transfer_buffer[transfer_buffer_index++] = value;
        if (transfer_buffer_index == sizeof(transfer_buffer)) {
            write();
            transfer_buffer_index = 0;
        }
        break;
    }
}

/* NOTE motorola big endianness, offsets 0-1 is block number msb lsb */
Byte diskio::get(Word address)
{
//    fprintf(stderr, "DISKIO: %1X->\r\n", address);
    switch(address) {
    case 0:
        return (block_number & 0xFF00) >> 8;
    case 1:
        return block_number & 0xFF;
    case 2:
        return disk_number;
    case 3:
        if (transfer_buffer_index == 0) {
            read();
        }
        return transfer_buffer[transfer_buffer_index++];
    }
    return 0;
}

/*
 * Sets the msb or lsb of target as value.
 * Offset is Motorola byte order => 0 = MSB, 1 = LSB
 */
void diskio::setWord(Byte offset, Word *target, Byte value)
{
    if (offset) { /* 0 or 1 */
        *target = (*target & 0xFF00) | (value & 0xFF); /* LSB */
    } else {
        *target = (value << 8) | (*target & 0xFF); /* MSB */
    }
}

void diskio::read()
{
    ssize_t read_bytes = 0;
    if (position() == 0) {
//        fprintf(stderr, "DISKIO: reading disk %d\r\n", disk_number);
        read_bytes = ::read(fd[disk_number], transfer_buffer, sizeof(transfer_buffer));
        if (read_bytes < 0) {
            fprintf(stderr, "DISKIO: read failed: %s\r\n", strerror(errno));
            read_bytes = 0;
        }
    }
    if (read_bytes < sizeof(transfer_buffer)) {
        memset(transfer_buffer+read_bytes, '~', sizeof(transfer_buffer)-read_bytes);
    }
}

void diskio::write()
{
    if (position() == 0) {
//        fprintf(stderr, "DISKIO: writing disk %d\r\n", disk_number);
        ::write(fd[disk_number], transfer_buffer, sizeof(transfer_buffer));
    }
}

/* Return -1 if fails, 0 if ok */
int diskio::position()
{
    static char filename[128];
    if (fd[disk_number] == 0) {
        sprintf(filename, "disk%d.img", disk_number);
        fd[disk_number] = open(filename, O_RDWR|O_CREAT);
    }
    if (fd[disk_number] < 0) { /* open has failed */
        fprintf(stderr, "DISKIO: open %s failed: %s\r\n", filename, strerror(errno));
        return fd[disk_number];
    }
    if (lseek(fd[disk_number], (size_t)block_number * (size_t)1024, SEEK_SET) < 0) {
        fprintf(stderr, "DISKIO: lseek block %d failed: %s\r\n", block_number, strerror(errno));
        return -1;
    }
    return 0;
}
