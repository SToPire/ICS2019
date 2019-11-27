#include "common.h"
#include <amdev.h>
int screen_width();
int screen_height();
void draw_sync();

size_t serial_write(const void* buf, size_t offset, size_t len)
{
    for (int i = 0; i < len; i++)
        _putc(*(char*)(buf + i));
    return len;
}

#define NAME(key) \
    [_KEY_##key] = #key,

static const char* keyname[256] __attribute__((used)) = {
    [_KEY_NONE] = "NONE",
    _KEYS(NAME)};

size_t events_read(void* buf, size_t offset, size_t len)
{
    int getkey = read_key();
    bool keydown = false;
    if (getkey & 0x8000) {
        keydown = true;
        getkey ^= 0x8000;
    }
    if (getkey == _KEY_NONE)
        return snprintf(buf, len, "t %u\n", uptime());
    else
        return snprintf(buf, len, "%s %s\n", keydown ? "kd" : "ku", keyname[getkey]);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void* buf, size_t offset, size_t len)
{
    if (len + offset > 128)
        len = 128 - offset;
    strncpy(buf, dispinfo + offset, len);
    return len;
}

size_t fb_write(const void* buf, size_t offset, size_t len)
{
    int offset_B = offset / 4;
    int len_B = len / 4;
    int w = screen_width();

    int x = offset_B % w;
    int y = offset_B / w;

    int len_first_line = (len_B > w - x) ? (w - x) : len_B;
    draw_rect((uint32_t*)buf, x, y, len_first_line, 1);

    int len_middle_line = 0;
    if (len_first_line + w < len_B) {
        draw_rect((uint32_t*)buf + len_first_line, 0, y + 1, w, (len_B - len_first_line) / w);
        len_middle_line = w * (len_B - len_first_line) / w;
        y += (len_B - len_first_line) / w;
    }

    if (len_first_line + len_middle_line < len_B) {
        draw_rect((uint32_t*)buf + len_first_line + len_middle_line, 0, y + 1, len_B - len_first_line - len_middle_line, 1);
    }
    return len;
}

size_t fbsync_write(const void* buf, size_t offset, size_t len)
{
    draw_sync();
    return len;
}

void init_device()
{
    Log("Initializing devices...");
    _ioe_init();
    sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}