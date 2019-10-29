#ifndef MC6809_DEBUG_H
#define MC6809_DEBUG_H

#include "mc6809.h"

class mc6809_debug : public mc6809
{
public:
    mc6809_debug();
    void run(void);
    void status(void);
protected:
    void dr(const char *prefix, Word reg);
    void db(const char *prefix, Byte value);
    void dump(void);
private:
    int steps;
    int previous_steps;
};

#endif // MC6809_DEBUG_H
