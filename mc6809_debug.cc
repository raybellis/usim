#include "mc6809_debug.h"
#include <cstdio>
#include <cstdlib>
#include "misc.h"

mc6809_debug::mc6809_debug() :
    mc6809(),
    steps(1),
    previous_steps(1)
{
}

void mc6809_debug::run()
{
    halted = 0;
	while (!halted) {
		execute();
        status();
    }
}

void mc6809_debug::status()
{
    dump();
    if (--steps == 0) {
        char buffer[80];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
             halted = 1;
        }
        switch (buffer[0]) {
        default:
            steps = previous_steps;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            steps = previous_steps = atoi(buffer);
            break;
        case 'x':
        case 'e':
        case 4:
            halted = 1;
            break;
        }
    }
}

void mc6809_debug::dr(const char *prefix, Word reg) {
  fprintf(stderr, "%s:%s", prefix, hexstr(reg));
  fprintf(stderr, "->%s", hexstr(read_word(reg)));
}
void mc6809_debug::db(const char *prefix, Byte value) {
  fprintf(stderr, "%s:%s", prefix, hexstr(value));
}

void mc6809_debug::dump()
{
  dr("PC", pc);
  dr(" S", s);
  dr(" U", u);
  dr(" Y", y);
  dr(" X", x);
  db(" A",a);
  db(" B",b);
  fprintf(stderr, "\r\n");
}
