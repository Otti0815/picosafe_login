#include <util/atomic.h>

#include <stdint.h>
#include "fifo.h"

void fifo_init (fifo_t *fifo, uint8_t *buffer, const uint8_t size)
{
  fifo->count = 0;
  fifo->pread = fifo->pwrite = buffer;
  fifo->read2end = fifo->write2end = fifo->size = size;;
}

uint8_t fifo_puts(fifo_t *fifo, const uint8_t *data, const uint8_t len)
{
  uint8_t i;

  for(i = 0; i < len; i++)
  {
    if(!fifo_put(fifo, data[i]))
      break;
  }

  return i;
}

uint8_t fifo_put(fifo_t *fifo, const uint8_t data)
{
  uint8_t write2end;
  uint8_t *pwrite;
  uint8_t ret;

  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    if (fifo->count >= fifo->size)
      ret = 0;
    else
    {
      pwrite = fifo->pwrite;
      
      *(pwrite++) = data;
      
      write2end = fifo->write2end;
      
      if (--write2end == 0)
      {
        write2end = fifo->size;
        pwrite -= write2end;
      }
      
      fifo->write2end = write2end;
      fifo->pwrite = pwrite;
      fifo->count++;

      ret = 1;
    }
  }

	return ret;
}

uint8_t fifo_get(fifo_t *fifo, uint8_t *data)
{
	uint8_t *pread;
  uint8_t read2end, count, ret;
  
  ATOMIC_BLOCK(ATOMIC_FORCEON)
  {
    count = fifo->count;

    if(count)
    {
      pread = fifo->pread;
      *data = *(pread++);
      read2end = fifo->read2end;
      
      if (--read2end == 0)
      {
        read2end = fifo->size;
        pread -= read2end;
      }
      
      fifo->pread = pread;
      fifo->read2end = read2end;

      fifo->count--;

      ret = 1;
    }
    else
      ret = 0;
  }
	
	return ret;
}

uint8_t fifo_get_wait(fifo_t *fifo)
{
  uint8_t data;

  while(!fifo_get(fifo, &data));

  return data;
}
