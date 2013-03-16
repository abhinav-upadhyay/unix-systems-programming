#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

typedef struct list_struct {
    data_t item;
    struct list_struct *next;
} log_t;

static log_t *headptr = NULL;
static log_t *tailptr = NULL;

int
addmsg(data_t data)
{
    size_t nodesize = sizeof(data_t) + strlen(data.string) + 1;
    int error;
    log_t *new_node = malloc(nodesize);
    if (new_node == NULL) {
        error = errno;
        warnx("malloc failed");
        errno = error;
        return -1;
    }
    new_node->item.time = data.time;
    new_node->item.string = (char *) new_node + sizeof(log_t); 
    memcpy(new_node->item.string, data.string, strlen(data.string) + 1);
    new_node->next = NULL;
    if (headptr == NULL) {
        headptr = new_node;
    } else {
        tailptr->next = new_node;
    }
    tailptr = new_node;
    return 0;
}

void
clearlog(void)
{
    log_t *next = headptr;
    while (next != NULL) {
        next = headptr->next;
        free(headptr);
        headptr = next;
    }
}

char *
getlog(void)
{
    size_t len;
    size_t msglen;
    size_t offset;
    log_t *node = headptr;
    char *log;
    while (node != NULL) {
        len += strlen(node->item.string);
        len++;
        node = node->next;
    }

    log = malloc(len);
    if (log == NULL) {
        warn("malloc failed while allocating memory for getlog");
        return NULL;
    }

    node = headptr;
    offset = 0;
    while(node) {
        msglen = strlen(node->item.string);
        memcpy(log + offset, node->item.string, msglen);
        offset += msglen;
        memcpy(log + offset, "\n", 1);
        offset++;
    }
    memcpy(log + offset - 1, "\0", 1);
    return log;
}

int
savelog(const char *filename)
{
    FILE *f;
    int error;
    log_t *node;
    if (filename == NULL) {
        errno = EINVAL;
        return -1;
    }

    f = fopen(filename, "w");
    if (f == NULL) {
        error = errno;
        warn("Failed to open %s", filename);
        errno = error;
        return -1;
    }

    node = headptr;
    while (node) {
        char *time = ctime(&node->item.time);
        time[strlen(time) - 1] = 0;
        fprintf(f, "%s: %s\n", time, node->item.string);
        node = node->next;
    }
    if (fclose(f)) {
        error = errno;
        warn("fclose failed in savelog");
        errno = error;
        return -1;
    }
    return 0;
}
