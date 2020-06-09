#ifndef AS_TABLE_H
#define AS_TABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"

typedef struct {
  char label[MAX_LINE_LENGTH];
  uint16_t address;
} label_pair;

typedef struct {
  int length;
  label_pair **labels;
} label_dict;

label_dict *new_dict();

void add(const char *label,uint16_t adress,label_dict *dict);

uint16_t query(const char *label,label_dict *dict);

void free_dict(label_dict *dict);

#endif
