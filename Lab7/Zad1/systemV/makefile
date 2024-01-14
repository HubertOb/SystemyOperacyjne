all:
	touch key.txt
	touch mem.txt
	gcc main_systemV.c -o main
	gcc client_systemV.c -o client
	gcc hairdresser_systemV.c -o hairdresser
	
	@echo Aby przetestować należy najpierw uruchomić main, następnie odpowiednią liczbę fryzjerów a nastepnie uruchamiac klientów. Proces klienta po zakończeniu sam się zamyka. Procesy main i fryzjer trzeba zamknąć sygnałem SIGINT. Stałe odnośnie liczby foteli, fryzjerów, fryzur i miejsc w poczekalni są zdefiniowane w pliku consts.h. Pliki key.txt i mem.txt są plikami pomocniczymi, w oparciu o te pliki generowane są klucze.
	
clean:
	rm -rf key.txt
	rm -rf mem.txt
	rm -rf main
	rm -rf client
	rm -rf hairdresser
