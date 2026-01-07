#include "pipe.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Funkcia na otvorenie dátovodu s príslušnými značkami
static void pipe_open_with_flags(pipe_data_t * this, int flags) {
  this->fD = open(this->path, flags);
  if (this->fD == -1) {
    fprintf(stderr, "pipe_open_with_flags: zlyhanie otvorenia datovodu %s", this->path);
    perror("");
    exit(EXIT_FAILURE);
  }
}
// Funkcia na inicializáciu dátovotu, pričom je potrebné uviesť jeho názov, resp. cestu a to, či je potrebné dátovod vytvoriť
void pipe_init(pipe_data_t * this, char * path, _Bool withCreation) {
  if (withCreation) {
    if (mkfifo(path, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP) == -1) {
      perror("pipe_init: zlyhanie pri vytvoreni datovodu!");
      exit(EXIT_FAILURE);
    }
  }
  this->path = path;
  this->fD = 0;
}
// Funkcia na zničenie dátovodu, pričom je potrebné uviesť, či má dôjsť aj k jeho vymazaniu a či je potrebné uvoľniť pamäť, v ktorej je vytvorený názov dátovodu
void pipe_destroy(pipe_data_t * this, _Bool withDeletion, _Bool withNameFree) {
  if (withDeletion) {
    if (unlink(this->path) == -1) {
      if (withNameFree) {
        free(this->path);
      }
      perror("pipe_destroy: zlyhanie funkcie unlink pri vymazavani datovodu!");
      exit(EXIT_FAILURE);
    }
  }
  if (withNameFree) {
    free(this->path);
  }
  this->path = NULL;
  this->fD = 0;
}
// Funkcia na otvorenie dátovodu pre zápis (značka O_WRONLY)
void pipe_open_write(pipe_data_t * this) {
  pipe_open_with_flags(this, O_WRONLY);
}
// Funkcia na otvorenie dátovodu na čítanie (značka O_RDONLY)
void pipe_open_read(pipe_data_t * this) {
  pipe_open_with_flags(this, O_RDONLY);
}
// Funkcia pre čítanie z dátovodu, pričom je potrebné uviesť aj miesto, kde sa uloží výsledok a maximálnu veľkosť miesta na uloženie
void pipe_read(pipe_data_t * this, char * result, size_t size) {
  if (read(this->fD, result, size) == -1) {
    perror("pipe_read: zlyhalo citanie z datovodu!");
  }
}
// Funkcia pre zápis do dátovodu, pričom je potrebné uviesť aj miesto, kde sa nachádzajú údaje a veľkosť miesta, z ktorého sa číta
void pipe_write(pipe_data_t * this, const char * data, size_t size) {
  if (write(this->fD, data, size) == -1) {
    perror("pipe_write: zlyhal zapis do datovodu!");
  }
}
// Funkcia na zavretie dátovodu
void pipe_close(pipe_data_t * this) {
  if (close(this->fD) == -1) {
    perror("pipe_close: zlyhanie pri zatvarani datovodu!");
    exit(EXIT_FAILURE);
  }
  this->fD = 0;
}
