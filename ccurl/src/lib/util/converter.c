#include "converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define HASH_LENGTH 243
#define TRYTE_SPACE 27
#define MIN_TRYTE_VALUE -13
#define MAX_TRYTE_VALUE = 13
#define RADIX 3
#define MAX_TRIT_VALUE (RADIX - 1) / 2
#define MIN_TRIT_VALUE -MAX_TRIT_VALUE
#define NUMBER_OF_TRITS_IN_A_BYTE 5
#define NUMBER_OF_TRITS_IN_A_TRYTE 3
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

//#define mytrits[NUMBER_OF_TRITS_IN_A_BYTE]
// char trits[NUMBER_OF_TRITS_IN_A_BYTE];
//#define IN_BYTE_TO_TRITS[HASH_LENGTH][]
//#define IN_TRYTE_TO_TRITS[HASH_LENGTH][]

/*
#define BLANK_BYTE_MAPPINGS
{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}
#define BLANK_TRYTE_MAPPINGS
{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
*/

char BYTE_TO_TRITS_MAPPINGS
    [HASH_LENGTH][NUMBER_OF_TRITS_IN_A_BYTE]; /*[HASH_LENGTH] = (char *[]){
                                                 BLANK_BYTE_MAPPINGS };*/
char TRYTE_TO_TRITS_MAPPINGS
    [TRYTE_SPACE][NUMBER_OF_TRITS_IN_A_TRYTE]; /*[TRYTE_SPACE] = (char *[]){
                                                  BLANK_TRYTE_MAPPINGS };*/

static const char* TRYTE_ALPHABET = TRYTE_STRING;

char long_value(char* const trits, const int offset, const int size) {
  int i;

  char value = 0;
  for (i = size; i-- > 0;) {
    value = value * RADIX + trits[offset + i];
  }
  return value;
}

char* bytes_from_trits(char* const trits, const int offset, const int size) {
  int i, j;
  int length =
      (size + NUMBER_OF_TRITS_IN_A_BYTE - 1) / NUMBER_OF_TRITS_IN_A_BYTE;
  char* bytes = (char*)malloc(sizeof(char) * length);
  for (i = 0; i < length; i++) {

    char value = 0;
    for (j = (size - i * NUMBER_OF_TRITS_IN_A_BYTE) < 5
                     ? (size - i * NUMBER_OF_TRITS_IN_A_BYTE)
                     : NUMBER_OF_TRITS_IN_A_BYTE;
         j-- > 0;) {
      value = value * RADIX + trits[offset + i * NUMBER_OF_TRITS_IN_A_BYTE + j];
    }
    bytes[i] = (char)value;
  }

  return bytes;
}

void getTrits(const char* bytes, int bytelength, char* const trits,
              int length) {
  int i;

  int offset = 0;
  for (i = 0; i < bytelength && offset < length; i++) {
    memcpy(
        trits + offset,
        BYTE_TO_TRITS_MAPPINGS
            [bytes[i] < 0
                 ? (bytes[i] +
                    HASH_LENGTH /* length of what? first? BYTE_TO_TRITS_MAPPINGS.length */)
                 : bytes[i]],
        sizeof(char) * (length - offset < NUMBER_OF_TRITS_IN_A_BYTE
                              ? (length - offset)
                              : NUMBER_OF_TRITS_IN_A_BYTE));
    offset += NUMBER_OF_TRITS_IN_A_BYTE;
  }
  while (offset < length) {
    trits[offset++] = 0;
  }
}

char* trits_from_trytes(const char* trytes, int length) {
  int i;
  char* trits = malloc(length * NUMBER_OF_TRITS_IN_A_TRYTE * sizeof(char));
  for (i = 0; i < length; i++) {
    memcpy(trits + i * NUMBER_OF_TRITS_IN_A_TRYTE,
           TRYTE_TO_TRITS_MAPPINGS[strchr(TRYTE_ALPHABET, trytes[i]) -
                                   TRYTE_ALPHABET],
           sizeof(char) * NUMBER_OF_TRITS_IN_A_TRYTE);
  }

  return trits;
}

void copyTrits(char const value, char* const destination, const int offset,
               const int size) {
  int i;

  char absoluteValue = value < 0 ? -value : value;
  for (i = 0; i < size; i++) {

    int remainder = (int)(absoluteValue % RADIX);
    absoluteValue /= RADIX;
    if (remainder > MAX_TRIT_VALUE) {

      remainder = MIN_TRIT_VALUE;
      absoluteValue++;
    }
    destination[offset + i] = remainder;
  }

  if (value < 0) {
    for (i = 0; i < size; i++) {
      destination[offset + i] = -destination[offset + i];
    }
  }
}

char* trytes_from_trits(char* const trits, const int offset, const int size) {
  int i;
  const int length =
      (size + NUMBER_OF_TRITS_IN_A_TRYTE - 1) / NUMBER_OF_TRITS_IN_A_TRYTE;
  char* trytes = malloc(sizeof(char) * (length + 1));
  trytes[length] = '\0';

  for (i = 0; i < length; i++) {
    char j = trits[offset + i * 3] + trits[offset + i * 3 + 1] * 3 +
               trits[offset + i * 3 + 2] * 9;
    if (j < 0) {
      j += 27;
    }
    trytes[i] = TRYTE_ALPHABET[(size_t)j];
  }
  return trytes;
}

char tryteValue(char* const trits, const int offset) {
  return trits[offset] + trits[offset + 1] * 3 + trits[offset + 2] * 9;
}

static void increment(char* trits, int size) {
  int i;
  for (i = 0; i < size; i++) {
    if (++trits[i] > MAX_TRIT_VALUE) {
      trits[i] = MIN_TRIT_VALUE;
    } else {
      break;
    }
  }
}

void init_converter (void) __attribute__ ((constructor));
void init_converter() {
  int i;
  char trits[NUMBER_OF_TRITS_IN_A_BYTE];
  memset(trits, 0, NUMBER_OF_TRITS_IN_A_BYTE * sizeof(char));
  for (i = 0; i < HASH_LENGTH; i++) {
    memcpy(&(BYTE_TO_TRITS_MAPPINGS[i]), trits,
           NUMBER_OF_TRITS_IN_A_BYTE * sizeof(char));
    increment(trits, NUMBER_OF_TRITS_IN_A_BYTE);
  }
  for (i = 0; i < TRYTE_SPACE; i++) {
    memcpy(&(TRYTE_TO_TRITS_MAPPINGS[i]), trits,
           NUMBER_OF_TRITS_IN_A_TRYTE * sizeof(char));
    increment(trits, NUMBER_OF_TRITS_IN_A_TRYTE);
  }
}
