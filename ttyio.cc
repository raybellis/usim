#include "ttyio.h"

void ttyio::set(Word address, Byte value)
{
    switch (address) {
    case 1:
        return term.write(value);
    }
}

Byte ttyio::get(Word address)
{
    switch (address) {
    case 0:
        return term.poll() != 0;
    case 1:
        return term.read();
    }
    return 0;
}
