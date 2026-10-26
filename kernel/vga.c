// vga.c
#include "vga.h"
#include <stdarg.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
static volatile uint16_t *const VGA = (uint16_t*)0xB8000;
static int row = 0, col = 0;
static uint8_t color = 0x0F;

static uint16_t entry(char c) { return ((uint16_t)color << 8) | (uint8_t)c; }

void vga_set_color(uint8_t fg, uint8_t bg) {
    color = (bg << 4) | (fg & 0x0F);
}

void vga_clear(void) {
    for (int i = 0; i < VGA_W * VGA_H; i++) VGA[i] = entry(' ');
    row = col = 0;
}

static void scroll(void) {
    if (row < VGA_H) return;
    for (int i = 0; i < (VGA_H - 1) * VGA_W; i++) VGA[i] = VGA[i + VGA_W];
    for (int i = (VGA_H - 1) * VGA_W; i < VGA_H * VGA_W; i++) VGA[i] = entry(' ');
    row = VGA_H - 1;
}

void vga_putc(char c) {
    if (c == '\n') { col = 0; row++; }
    else if (c == '\r') { col = 0; }
    else if (c == '\t') { col = (col + 4) & ~3; }
    else {
        VGA[row * VGA_W + col] = entry(c);
        if (++col >= VGA_W) { col = 0; row++; }
    }
    scroll();
}

void vga_puts(const char *s) { while (*s) vga_putc(*s++); }

// Минимальный printf: %s %c %d %x %%
static void put_uint(uint32_t v, int base, int pad) {
    char buf[32]; int i = 0;
    const char *dig = "0123456789abcdef";
    if (v == 0) buf[i++] = '0';
    while (v) { buf[i++] = dig[v % base]; v /= base; }
    while (i < pad) buf[i++] = '0';
    while (i--) vga_putc(buf[i]);
}

void vga_printf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { vga_putc(*fmt); continue; }
        fmt++;
        switch (*fmt) {
            case 's': vga_puts(va_arg(ap, const char*)); break;
            case 'c': vga_putc((char)va_arg(ap, int)); break;
            case 'd': { int x = va_arg(ap, int);
                        if (x < 0) { vga_putc('-'); x = -x; }
                        put_uint(x, 10, 0); break; }
            case 'x': put_uint(va_arg(ap, uint32_t), 16, 0); break;
            case '%': vga_putc('%'); break;
        }
    }
    va_end(ap);
}
