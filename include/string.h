#ifndef MPX_STRING_H
#define MPX_STRING_H

#include <stddef.h>

/**
 @file string.h
 @brief A subset of standard C library functions.
*/

/**
 Copy a region of memory.
 @param dst The destination memory region
 @param src The source memory region
 @param n The number of bytes to copy
 @return A pointer to the destination memory region
*/
void* memcpy(void * restrict dst, const void * restrict src, size_t n);

/**
 Fill a region of memory.
 @param address The start of the memory region
 @param c The byte to fill memory with
 @param n The number of bytes to fill
 @return A pointer to the filled memory region
*/
void* memset(void *address, int c, size_t n);

/**
 Compares two strings
 @param s1 The first string to compare
 @param s2 The second string to compare
 @return 0 if strings are equal, <0 if s1 is lexicographically before s2, >0 otherwise
*/
int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, unsigned int n);

/**
 Returns the length of a string.
 @param s A NUL-terminated string
 @return The number of bytes in the string (not counting NUL terminator)
*/
size_t strlen(const char *s);

/**
 * @brief Split string into tokens.
 *
 * @param s1 String to tokenize (NULL to continue tokenizing).
 * @param s2 Delimiter characters.
 * @return Pointer to next token, or NULL if none.
 */
char* strtok(char * restrict s1, const char * restrict s2);

/**
 * @brief Copy a string with length limit.
 *
 * @param dest Destination buffer.
 * @param src Source string.
 * @param num_of_chars Maximum number of characters to copy.
 * @return Pointer to destination buffer.
 */
char* strncpy(char* dest, const char* src, unsigned int num_of_chars);
#endif
