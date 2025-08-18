#include "id.h"

unsigned int getNextId() {
    static unsigned int nxtID = 0;
    return nxtID++;
}