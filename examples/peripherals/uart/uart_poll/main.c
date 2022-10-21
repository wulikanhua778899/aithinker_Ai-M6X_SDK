#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s *uart1;

int main(void)
{
    board_init();
    board_uart1_gpio_init();

    uart1 = bflb_device_get_by_name("uart1");

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uart1, &cfg);

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uart1);
        if (ch != -1) {
            bflb_uart_putchar(uart1, ch);
        }
    }
}