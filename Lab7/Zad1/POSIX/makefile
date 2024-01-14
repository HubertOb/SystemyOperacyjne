all:
	gcc -pthread main_POSIX.c -o main -lrt
	gcc -pthread client_POSIX.c -o client -lrt
	gcc -pthread hairdresser_POSIX.c -o hairdresser -lrt
	
	@echo Aby przetestować należy najpierw uruchomić main, następnie odpowiednią liczbę fryzjerów a nastepnie uruchamiac klientów. Proces klienta po zakończeniu sam się zamyka. Procesy main i fryzjer trzeba zamknąć sygnałem SIGINT. Stałe odnośnie liczby foteli, fryzjerów, fryzur i miejsc w poczekalni są zdefiniowane w pliku consts2.h.
	
clean:
	rm -rf main
	rm -rf client
	rm -rf hairdresser
