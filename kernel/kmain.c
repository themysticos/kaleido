#include <stdint.h>
#include "vga.h"
#include "market.h"

// демо-возможность: эхо
static void *echo_fn(void *arg) {
    vga_printf("  [echo cell] получил аргумент 0x%x\n", (uint32_t)(uintptr_t)arg);
    return arg;
}

void kmain(uint32_t magic, uint32_t mb_info) {
    vga_clear();
    vga_set_color(VGA_LGREEN, VGA_BLACK);
    vga_puts("=== KaleidoKernel prototype ===\n\n");
    vga_set_color(VGA_WHITE, VGA_BLACK);

    if (magic != 0x2BADB002) {
        vga_puts("[!] not loaded by Multiboot\n");
        return;
    }
    vga_printf("multiboot magic ok, mb_info = 0x%x\n", mb_info);

    market_init();
    int id = market_register("echo", echo_fn, 0);
    vga_printf("registered capability 'echo' id=%d\n", id);

    void *ctx = 0;
    cap_fn echo = market_acquire("echo", &ctx);
    if (echo) {
        vga_puts("acquired 'echo', calling...\n");
        echo((void*)0xCAFE);
    } else {
        vga_puts("[!] capability not found\n");
    }

    vga_set_color(VGA_YELLOW, VGA_BLACK);
    vga_puts("\nKernel halted. QEMU: press Ctrl+A X to quit.\n");
    for (;;) __asm__ volatile ("hlt");
}
