all: spaceship

.PHONY: all

LDFLAGS = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

spaceship: spaceship.o nev_bekero.o jatek.o game_over.o scene.o
	g++ -o $@ $^ $(LDFLAGS)

spaceship.o: spaceship.cc nev_bekero.h jatek.h
	g++ -Wall -c $<

scene.o: scene.cc scene.h
	g++ -Wall -c $<

nev_bekero.o: nev_bekero.cc nev_bekero.h scene.h
	g++ -Wall -c $<

jatek.o: jatek.cc jatek.h scene.h
	g++ -Wall -c $<

game_over.o: game_over.cc game_over.h scene.h
	g++ -Wall -c $<


