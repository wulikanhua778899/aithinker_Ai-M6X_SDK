#include "bflb_uart.h"
#include "stdarg.h"

struct bflb_device_s *console = NULL;

int puts(const char *c)
{
    int len;

    if (c == NULL) {
        return 0;
    }

    len = strlen(c);

    if (console == NULL) {
        return 0;
    }

    bflb_uart_put(console, (uint8_t *)c, len);

    return len;
}

int putstring(const char *c)
{
    int len;

    if (c == NULL) {
        return 0;
    }

    len = strlen(c);

    if (console == NULL) {
        return 0;
    }

    bflb_uart_put(console, (uint8_t *)c, len);

    return len;
}

#if defined(CONFIG_VSNPRINTF_NANO) && CONFIG_VSNPRINTF_NANO
int printf(const char *fmt, ...)
{
    char print_buf[512];
    int len;
    va_list ap;

    if (console == NULL) {
        return 0;
    }

    va_start(ap, fmt);
    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);
    va_end(ap);

    len = (len > sizeof(print_buf)) ? sizeof(print_buf) : len;

    bflb_uart_put(console, (uint8_t *)print_buf, len);

    return len;
}
#else
extern int console_vsnprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...)
{
    int len;
    va_list ap;

    if (console == NULL) {
        return 0;
    }

    va_start(ap, fmt);
    len = console_vsnprintf(fmt, ap);
    va_end(ap);

    return len;
}
#endif

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void bflb_dump_hex(const void *ptr, uint32_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    for (i = 0; i < buflen; i += 16) {
        printf("%08X:", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen) {
                if ((j % 8) == 0) {
                    printf("  ");
                }

                printf("%02X ", buf[i + j]);
            } else
                printf("   ");
        printf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        printf("\n");
    }
}

// void bflb_dump_hex(uint8_t *data, uint32_t len)
// {
//     uint32_t i = 0;

//     for (i = 0; i < len; i++) {
//         if (i % 16 == 0) {
//             printf("\r\n");
//         }

//         printf("%02x ", data[i]);
//     }

//     printf("\r\n");
// }

void bflb_reg_dump(uint32_t addr)
{
    printf("%08lx[31:0]=%08lx\r\n", addr, *(volatile uint32_t *)(addr));
}

int bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            return -1;
        }
    }

    return 0;
}

void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
}