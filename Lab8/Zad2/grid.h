#pragma once
#include <stdbool.h>
#include <pthread.h>

typedef struct{
    int start;
    int stop;
}Args;


char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
pthread_t *init_threads(char* foreground, char* background, int threads_count);
void *foo(void* index);
void delete_pthreads(pthread_t *threads);