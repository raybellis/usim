#include "sidforth.h"
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

sidforth::sidforth() :
    console(),
    uart(),
    xterm_pipe()
{
    console = new ttyio();
    int uart_fd = xterm();
    if (uart_fd >= 0) {
        uart = new ttyio(uart_fd, uart_fd);
    }
}

sidforth::~sidforth()
{
    if (console) {
        delete console;
    }
    if (uart) {
        delete uart;
    }
    if (xterm_pipe) {
        int pid;
        if (fscanf(xterm_pipe, "%d", &pid) == 1) {
            if (kill(pid, SIGTERM)) {
                perror("UART: failed to kill xterm");
            } else {
                fprintf(stderr, "Sent TERM to %d\r\n", pid);
            }
        } else {
            static char data[256];
            fread(data, 1, sizeof(data)-1, xterm_pipe);
            fprintf(stderr, "UART: failed to read pid from: [%s]\n", data);
        }
        pclose(xterm_pipe);
    } else {
        fprintf(stderr, "UART: no xterm pipe?\n");
    }
}

Byte sidforth::read(Word addr)
{
    switch (addr & 0xfff0) {
    case disk_address: // Disk
		return disk.get(addr - disk_address);
    case console_address:
        return console->get(addr - console_address);
    case uart_address:
        return uart->get(addr - uart_address);
    default:
        return INHERIT::read(addr);
    }
}

void sidforth::write(Word addr, Byte c)
{
    switch (addr & 0xfff0) {
    case disk_address: // Disk
		disk.set(addr - disk_address, c); return;
    case console_address:
        console->set(addr - console_address, c); return;
    case uart_address:
        uart->set(addr - uart_address, c); return;
    case halt_address:
        INHERIT::halt(); return;
    default:
        INHERIT::write(addr, c); return;
    }
}

// Open xterm and return ttyio with slave
int sidforth::xterm()
{
    int masterfd = posix_openpt(O_RDWR|O_NOCTTY);
    char *slave;
    if (masterfd < 0 || unlockpt(masterfd) || grantpt(masterfd) || (slave = ptsname(masterfd)) == NULL) {
        perror("UART: failed to pty");
        if (masterfd >= 0) {
            ::close(masterfd);
        }
        return -1;
    }

    int uartfd = open(slave, O_RDWR|O_NOCTTY);

    // launch an xterm that uses the pseudo-terminal master we have opened
    char xterm_command[100];
    sprintf(xterm_command, "LANG=C xterm -S%s/%d & echo $!", slave, masterfd);
    xterm_pipe = popen(xterm_command, "r");
    if (!xterm_pipe) {
        perror("UART: failed to start xterm");
        close(uartfd);
        close(masterfd);
        return -1;
    }
    return uartfd;
}
