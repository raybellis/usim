#include "diskio.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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

Byte diskio::get(Word address)
{
    switch(address) {
    case 0:
        return block_number & 0xFF;
    case 1:
        return (block_number & 0xFF00) >> 8;
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

void diskio::setWord(Byte offset, Word *target, Byte value)
{
    if (offset) { /* 0 or 1 */
        *target = (value << 8) | (*target & 0xFF);
    } else {
        *target = (*target & 0xFF00) | (value & 0xFF);
    }
}

void diskio::read()
{
    ssize_t read_bytes = 0;
    if (position() == 0) {
        read_bytes = ::read(fd[disk_number], transfer_buffer, sizeof(transfer_buffer));
        if (read_bytes < 0) {
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
        ::write(fd[disk_number], transfer_buffer, sizeof(transfer_buffer));
    }
}

/* Return -1 if fails, 0 if ok */
int diskio::position()
{
    if (fd[disk_number] == 0) {
        char filename[128];
        sprintf(filename, "disk%d.img", disk_number);
        fd[disk_number] = open(filename, O_RDWR|O_CREAT);
    }
    if (fd[disk_number] < 0) { /* open has failed */
        return fd[disk_number];
    }
    if (lseek(fd[disk_number], (size_t)block_number * (size_t)1024, SEEK_SET) < 0) {
        return -1;
    }
    return 0;
}
