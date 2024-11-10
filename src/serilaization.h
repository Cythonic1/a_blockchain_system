#ifndef SERILIZATION_H
#define  SERILIZATION_H
#include "./block.h"


unsigned char *serilized(Block *block);
Block *deserilized(unsigned char *buffer);
#endif
