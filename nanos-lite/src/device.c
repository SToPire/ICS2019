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
    // strncpy(buf, dispinfo + offset, len);
    // return len;
    return 0;
}

size_t fb_write(const void* buf, size_t offset, size_t len)
{
    return 0;
}

size_t fbsync_write(const void* buf, size_t offset, size_t len)
{
    // draw_sync();
    return 0;
}

void init_device()
{
    Log("Initializing devices...");
    _ioe_init();
    // sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
