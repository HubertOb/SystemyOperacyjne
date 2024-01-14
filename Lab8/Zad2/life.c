#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

int main(int argc, char*argv[])
{
    if (argc!=2){
        printf("Niepoprawna liczba danych wejściowych\n");
        return 0;
    }
    int threads_count= strtol(argv[1],NULL,10);
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();

	init_grid(foreground);

    pthread_t *threads=init_threads(foreground,background,threads_count);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
        for (int i=0;i<threads_count;i++){
            pthread_kill(threads[i],SIGUSR1);
        }
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);
    delete_pthreads(threads);

	return 0;
}
