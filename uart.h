/*#ifndef __UART_H__
#define __UART_H__
*/
#include <stdint.h>

struct __file {
    char *        buf;   /* buffer pointer */
    unsigned char unget; /* ungetc() buffer */
    uint8_t       flags; /* flags, see below */
#define __SRD     0x0001 /* OK to read */
#define __SWR     0x0002 /* OK to write */
#define __SSTR    0x0004 /* this is an sprintf/snprintf string */
#define __SPGM    0x0008 /* fmt string is in progmem */
#define __SERR    0x0010 /* found error */
#define __SEOF    0x0020 /* found EOF */
#define __SUNGET  0x040  /* ungetc() happened */
#define __SMALLOC 0x80   /* handle is malloc()ed */
#if 0
/* possible future extensions, will require uint16_t flags */
#define __SRW  0x0100 /* open for reading & writing */
#define __SLBF 0x0200 /* line buffered */
#define __SNBF 0x0400 /* unbuffered */
#define __SMBF 0x0800 /* buf is from malloc */
#endif
    int size; /* size of buffer */
    int len;  /* characters read or written so far */
    int (*put)(char,
               struct __file *); /* function to write one char to device */
    int (*get)(struct __file *); /* function to read one char from device */
    void *udata;                 /* User defined and accessible data. */
};

typedef struct __file FILE;

#define FDEV_SETUP_STREAM(p, g, f) \
    { .put = p, .get = g, .flags = f, .udata = 0, }


//#endif

