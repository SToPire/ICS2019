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
    strncpy(buf, dispinfo + offset, len);
    return len;
}

size_t fb_write(const void* buf, size_t offset, size_t len)
{
    int x, y;
    int len1, len2, len3;
    //offset /= 4;
    offset = offset >> 2;
    y = offset / screen_width();
    x = offset % screen_width();

    //Log("fb_write x:%d y:%d len:%d\n", x, y, len);
    //len /= 4;
    len = len >> 2;
    len1 = len2 = len3 = 0;

    // the first line
    len1 = len <= screen_width() - x ? len : screen_width() - x;
    draw_rect((uint32_t*)buf, x, y, len1, 1);

    // the middle line
    if (len > len1 && ((len - len1) > screen_width())) {
        //len2 = (len - len1) / _screen.width * _screen.width;
        len2 = len - len1;
        draw_rect((uint32_t*)buf + len1, 0, y + 1, screen_width(), len2 / screen_width());
    }

    // the lase line
    if (len - len1 - len2 > 0) {
        len3 = len - len1 - len2;
        draw_rect((uint32_t*)buf + len1 + len2, 0, y + len2 / screen_width() + 1, len3, 1);
    }
    return 0;
}

size_t fbsync_write(const void* buf, size_t offset, size_t len)
{
    draw_sync();
    return 0;
}

void init_device()
{
    Log("Initializing devices...");
    _ioe_init();
    sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
