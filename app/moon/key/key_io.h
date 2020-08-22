#ifndef _KEY_IO_H_
#define _KEY_IO_H_

#define KIO0            0x01
#define KIO1            0x02
#define KIO2            0x04
#define KIO3            0x08
#define KIO4            0x10
#define KIO5            0x20
#define KIO6            0x40
#define KIO7            0x80


void kio_init(void);
void kio_get_sta(void);
uint32_t kio_get_key(void);

#endif

