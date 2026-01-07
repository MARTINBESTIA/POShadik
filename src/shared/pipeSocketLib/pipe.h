#ifndef PIPE_H
#define PIPE_H
#include <stddef.h>
// Štruktúra obsahujúca informácie pre prácu s dátovodom
typedef struct PipeData {
    char * path;
    int fD;
} pipe_data_t;
// Hlavičky funkcií, ktoré sú verejne dostupné a pracujú s dátovodom
void pipe_init(pipe_data_t * this, char * path, _Bool withCreation);
void pipe_destroy(pipe_data_t * this, _Bool withDeletion, _Bool withNameFree);
void pipe_open_write(pipe_data_t * this);
void pipe_open_read(pipe_data_t * this);
void pipe_read(pipe_data_t * this, char * result, size_t size);
void pipe_write(pipe_data_t * this, const char * data, size_t size);
void pipe_close(pipe_data_t * this);
#endif