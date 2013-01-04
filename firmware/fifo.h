#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>

typedef struct
{
	uint8_t count;                // # Zeichen im Puffer
	uint8_t size;                 // Puffer-Größe
	uint8_t *pread;               // Lesezeiger
	uint8_t *pwrite;              // Schreibzeiger
	uint8_t read2end, write2end;  // # Zeichen bis zum Überlauf Lese-/Schreibzeiger
} fifo_t;

void fifo_init (fifo_t *fifo, uint8_t *buffer, const uint8_t size);
uint8_t fifo_put(fifo_t *fifo, const uint8_t data);
uint8_t fifo_puts(fifo_t *fifo, const uint8_t *data, const uint8_t len);
uint8_t fifo_get(fifo_t *fifo, uint8_t *data);
uint8_t fifo_get_wait(fifo_t *fifo);

#endif
