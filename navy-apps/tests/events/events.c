#include <stdio.h>

int main()
{
    FILE* fp = fopen("/dev/events", "r");
    int time = 0;
    printf("Start to receive events...\n");
    while (1) {
        char buf[256];
        char *p = buf, ch;
        //printf("%p:%p", buf, p);
        int i = 0;
        while ((ch = fgetc(fp)) != -1) {
            //printf("%d", ch);
            *p++ = ch;
            printf("%p:%p\n", buf, p);
            //printf("%c", *(p - 1));
            if (ch == '\n') {
                i++;
                *p = '\0';
                // printf("%p %p %d\n", p, buf, i);
                break;
            }
        }
        i = 0;
        //printf("%s", buf);
        int is_time = buf[0] == 't';
        time += is_time;
        if (!is_time) {
            // printf("receive event: %s", buf);
        } else if (time % 1024 == 0) {
            printf("receive time event for the %dth time: %s", time, buf);
        }
    }

    fclose(fp);
    return 0;
}
