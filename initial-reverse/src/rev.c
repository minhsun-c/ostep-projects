#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "header.h"

#define ERR_MALLOC "malloc failed\n"
#define CHECK_MALLOC(arg) {\
    if (!(arg)) { \
        fprintf(stderr, ERR_MALLOC); \
        exit(1); \
    } \
}
#define MEM_IDX(i) ((char **)mem->data)[(i)]
#define BUF_IDX(buf, i) ((char *)(buf)->data)[(i)]
#define OPTION_MEM 0
#define OPTION_LEFT 1
#define OPTION_RD 2

static int allocate(mem_t *mem, int option) 
{
    mem->capacity = (option == OPTION_RD) ? BUF_SIZE : 4;
    mem->used = 0;
    if (option == OPTION_MEM)
        mem->data = malloc(mem->capacity * sizeof(char *));
    else 
        mem->data = malloc(mem->capacity * sizeof(char));
    if (!mem->data) 
        return 0;
    return 1;
}

static int reallocate(mem_t *mem, int option)
{
    if (option == OPTION_MEM)
        mem->data = realloc(mem->data, mem->capacity * 2 * sizeof(char *));
    else 
        mem->data = realloc(mem->data, mem->capacity * 2 * sizeof(char));
    mem->capacity *= 2;
    if (!mem->data) 
        return 0;
    return 1;
}

static void add_line(mem_t *mem, mem_t *src) 
{
    MEM_IDX(mem->used) = (char *) malloc((src->used + 1) * sizeof(char));
    CHECK_MALLOC(MEM_IDX(mem->used));
    memcpy(MEM_IDX(mem->used), src->data, src->used);
    MEM_IDX(mem->used)[src->used] = '\0';
    mem->used ++;
    src->used = 0;
}

static void copy_to(mem_t *dest, mem_t *src, size_t size)
{
    memcpy(&BUF_IDX(dest, dest->used), &BUF_IDX(src, src->used), size);
    dest->used += size;
    BUF_IDX(dest, dest->used) = 0;
    src->used += size;
}

static void read_file(mem_t *mem, fd_t fd_in) 
{
    mem_t left;
    mem_t rd_buf;
    
    char *newline;

    size_t rd_len, nl;

    CHECK_MALLOC(allocate(&left, OPTION_LEFT));
    CHECK_MALLOC(allocate(&rd_buf, OPTION_RD));

    while ((rd_len = read(fd_in, (char *) rd_buf.data, rd_buf.capacity)) > 0) {
        rd_buf.used = 0;
        while (rd_len > 0) {
            newline = memchr(&BUF_IDX(&rd_buf, rd_buf.used), '\n', rd_len);
            nl = (newline) ? newline - ((char *) rd_buf.data + rd_buf.used) + 1 : rd_len;

            while (left.used + nl + 1 > left.capacity) {
                CHECK_MALLOC(reallocate(&left, 1));
            }

            copy_to(&left, &rd_buf, nl);
            rd_len -= nl;

            if (newline) {
                add_line(mem, &left);
            }
            if (mem->used == mem->capacity) {
                CHECK_MALLOC(reallocate(mem, OPTION_MEM));
            }
        }
    }

    if (left.used > 0) {
        add_line(mem, &left);
    }

    free(left.data);
    free(rd_buf.data);
}

static void write_file(mem_t *mem, fd_t fd_out)
{
    for (int i=mem->used - 1; i>=0; i--) {
        write(fd_out, MEM_IDX(i), strlen(MEM_IDX(i)));
        free(MEM_IDX(i));
    }
    free((char **) mem->data);
}

void reverse_file(fd_t *fd)
{
    mem_t mem;
    CHECK_MALLOC(allocate(&mem, OPTION_MEM));
    read_file(&mem, fd[0]);
    write_file(&mem, fd[1]);
}