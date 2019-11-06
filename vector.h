//////////////////////////////////////////////////////////////
// Downloaded end edited library under MIT license          //
// A Vector (dynamic array) implementation in C             //
// https://github.com/imjacobclark/vectorlib                //
//                                                          //
// (c) Jacob Clark, 2017                                    //
//                                                          //
// MIT License                                              //
//////////////////////////////////////////////////////////////

#include <stdbool.h>
#define VECTOR_INITIAL_CAPACITY 8

typedef struct {
    int size;
    int capacity;
    int *data;
} Vector;

void init(Vector *vector);
void append(Vector *vector, int value);
void prepend(Vector *vector, int value);
void delete(Vector *vector, int index);
void delete_value(Vector *vector, int value);
void set(Vector *vector, int index, int value);
void resize(Vector *vector);
void free_memory(Vector *vector);

int get(Vector *vector, int value);
int pop(Vector *vector);
int find_value(Vector *vector, int value);
int size(Vector *vector);
int capacity(Vector *vector);

bool is_empty(Vector *vector);

void print_vector(Vector *vector);
void swap_vector_values(Vector *vector, int position1, int position2);
void set_capacity(Vector *vector, int capacity);
void insert_to_index(Vector *vector, int index, int value);
