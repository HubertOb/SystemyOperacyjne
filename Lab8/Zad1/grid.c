#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;

char* bg;
char* fg;


void handler(int signum){

}

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void *foo(void *ind)
{
    int index=(int) ind;
    char *background=bg;
    char *foreground=fg;
    char* tmp;

    while (true){
        background[index]=is_alive(index/grid_width,index%grid_width,foreground);
        tmp=foreground;
        foreground=background;
        background=tmp;
        pause();
    }
}

pthread_t *init_threads(char* foreground, char* background)
{
    fg=foreground;
    bg=background;
    signal(SIGUSR1,handler);
    pthread_t *threads = malloc(sizeof(pthread_t) * grid_width * grid_height);
    for (int i=0;i<grid_width*grid_height;i++){
        pthread_create(&threads[i],NULL, foo,(void *)i);
    }
    return threads;
}

void destroy_grid(char *grid)
{
    free(grid);
}

void delete_pthreads(pthread_t *threads){
    free(threads);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}



