#ifndef ITOBCD_H
#define ITOBCD_H

/**
 @file itoBCD.h
 @brief Function that converts an integer into a string
* that is representative of the binary coded decimal
* format of the input integer.
*/

/**
 Convert an integer to an Binary Coded Decimal
 @param int Integer being converted into a Binary Coded Decimal
 @param s A buffer to hold the created string
*/

void itoBCD(int num, char* buffer);

#endif