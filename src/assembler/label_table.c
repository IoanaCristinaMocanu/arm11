#include "label_table.h"

label_dict *new_dict(){
  label_dict *dict = malloc(sizeof(label_dict));
  dict->length = 0;
  return dict;
}

void add(const char *label,uint16_t address,label_dict *dict){
  dict->length++;
  dict->labels = realloc(dict->labels,dict->length);
  label_pair *pair = malloc(sizeof(label_pair));
  pair->address = address;
  strcpy(pair->label,label);
  dict->labels[dict->length - 1] = pair;
}

uint16_t query(const char *label,label_dict *dict){
  for(int i = 0;i < dict->length - 1; i++){
    if(strcmp(label,dict->labels[i]->label) == 0){
      return dict->labels[i]->address;
    }
  }
  return 0;
}

void free_dict(label_dict *dict){
  for(int i = 0;i < dict->length - 1; i++){
    free(&dict->labels[i]);
  }
  free(dict);
}
