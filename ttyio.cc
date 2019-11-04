#include "ttyio.h"
#include <fcntl.h>

ttyio::ttyio(const char *input_device, const char *output_device)
{
    int in_fd = 0;
    int out_fd = 1;
    if (input_device) {
        in_fd = open(input_device, O_RDONLY|O_NONBLOCK);
        if (in_fd < 0) {
            perror(input_device);
        }
    }
    if (output_device) {
        out_fd = open(output_device, O_WRONLY|O_NONBLOCK);
        if (out_fd < 0) {
            perror(output_device);
        }
    }
    term = new Terminal(in_fd, out_fd);
}

ttyio::ttyio(int ifd, int ofd)
{
    term = new Terminal(ifd, ofd);
}

ttyio::~ttyio()
{
    delete term;
}

void ttyio::set(Word address, Byte value)
{
    switch (address) {
    case 1:
        return term->write(value);
    }
}

Byte ttyio::get(Word address)
{
    int read_ready;
    int write_ready;
    switch (address) {
    case 0:
        read_ready = term->poll_in();
        write_ready = term->poll_out();
        return (write_ready ? 2 : 0) | (read_ready ? 1 : 0);
    case 1:
        return term->read();
    }
    return 0;
}
