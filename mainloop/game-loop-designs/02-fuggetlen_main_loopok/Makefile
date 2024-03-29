all: spaceship

.PHONY: all

LDFLAGS = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

spaceship: spaceship.o nev_bekero.o jatek.o game_over.o
	g++ -o $@ $^ $(LDFLAGS)

spaceship.o: spaceship.cc nev_bekero.h jatek.h game_over.h
	g++ -Wall -c $<

nev_bekero.o: nev_bekero.cc nev_bekero.h
	g++ -Wall -c $<

jatek.o: jatek.cc jatek.h
	g++ -Wall -c $<

game_over.o: game_over.cc game_over.h
	g++ -Wall -c $<


