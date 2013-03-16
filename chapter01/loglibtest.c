#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

static void
logmsg(const char *msg)
{
    data_t data;
    time(&data.time);
    size_t msglen = strlen(msg);
    data.string = malloc(msglen + 1);
    memcpy(data.string, msg, msglen + 1);
    addmsg(data);
    free(data.string);
}
 

int
main(int argc, char **argv)
{
    logmsg("hello world");
    logmsg("ls -l");
    logmsg("cp");
    logmsg("mv");
    char *msgs = getlog();
    printf("%s\n", msgs);
    free(msgs);
    savelog("/tmp/log");
    clearlog();

    return 0;
}

