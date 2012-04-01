#ifndef CIRCBUFF_
#define CIRCBUFF_

#include <stdio.h>

/* Opaque buffer element type.  This would be defined by the application. */
typedef struct { void* value; } ElemType;
 
/* Circular buffer object */
typedef struct {
    int         size;   /* maximum number of elements           */
    int         start;  /* index of oldest element              */
    int         end;    /* index at which to write new element  */
    ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size);
 
void cbFree(CircularBuffer *cb);
 
int cbIsFull(CircularBuffer *cb);
int cbIsEmpty(CircularBuffer *cb);
 
/* Write an element, overwriting oldest element if buffer is full. App can
   choose to avoid the overwrite by checking cbIsFull(). */
void cbWrite(CircularBuffer *cb, ElemType *elem);
 
/* Read oldest element. App must ensure !cbIsEmpty() first. */
void cbRead(CircularBuffer *cb, ElemType *elem);

#endif /*CIRCBUFF_*/
