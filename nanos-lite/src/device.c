#include "common.h"
#include <amdev.h>

// extern int screen_width();
// extern int screen_height();
// extern void draw_rect(uint32_t *pixels, int x, int y, int w, int h);
// extern size_t fs_filesz(int fd);
extern int fs_open(const char* pathname, int flags, int mode);
extern size_t fs_read(int fd, void* buf, size_t len);
extern size_t fs_write(int fd, const void* buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);

size_t serial_write(const void* buf, size_t offset, size_t len)
{
    int ret = 0;
    for (; ret < len; ret++) _putc(((char*)buf)[ret]);
    return ret;
}

#define NAME(key) \
    [_KEY_##key] = #key,

static const char* keyname[256] __attribute__((used)) = {
    [_KEY_NONE] = "NONE",
    _KEYS(NAME)};

size_t events_read(void* buf, size_t offset, size_t len)
{
    char temp[1024];
    int key = read_key();

    bool down = false;
    if (key & 0x8000) {
        key ^= 0x8000;
        down = true;
    }

    if (key == _KEY_NONE) {
        uint32_t t = uptime();
        sprintf(temp, "t %d\n", t);
    } else {
        if (down)
            sprintf(temp, "%s %s\n", "kd", keyname[key]);
        else
            sprintf(temp, "%s %s\n", "ku", keyname[key]);
    }
    if (strlen(temp) < len) len = strlen(temp);

    memcpy(buf, temp, len);
    return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void* buf, size_t offset, size_t len)
{
    // printf("offset = %d, len = %d\n",offset, len);
    int filesz = 128;
    if (len + offset > filesz) len = filesz - offset;
    // printf("offset = %d, len = %d\n",offset, len);
    strncpy(buf, dispinfo + offset, len);
    // printf("%d %d\n",screen_width(),screen_height());
    return len;
}

size_t fb_write(const void* buf, size_t offset, size_t len)
{
    int x, y;
    offset >>= 2;
    int scr_width = screen_width();
    // int scr_height = screen_height();
    x = offset % scr_width;
    y = offset / scr_width;

    // printf("fb write: x=%d, y=%d\n",x,y);

    int res = len;

    len >>= 2;
    int len_t = len < scr_width - x ? len : scr_width - x;
    draw_rect((uint32_t*)buf, x, y, len_t, 1);

    if (len_t < len - scr_width && len_t < len) {
        draw_rect((uint32_t*)buf + len_t, 0, y + 1, scr_width, (len - len_t) / scr_width);
        len_t += scr_width * ((len - len_t) / scr_width);
        y += (len - len_t) / scr_width;
    }

    if (len_t < len) {
        draw_rect((uint32_t*)buf + len_t, 0, y + 1, len - len_t, 1);
    }

    return res;
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

    // TODO: print the string to array `dispinfo` with the format
    // described in the Navy-apps convention
    sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
    printf("%s", dispinfo);
}
