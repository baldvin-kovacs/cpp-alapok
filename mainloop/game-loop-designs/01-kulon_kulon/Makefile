all: \
	nev_bekero \
	jatek \
	game_over

.PHONY: all

LDFLAGS = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

nev_bekero: nev_bekero.cc
	g++ -Wall -o nev_bekero nev_bekero.cc $(LDFLAGS)

jatek: jatek.cc
	g++ -Wall -o jatek jatek.cc $(LDFLAGS)

game_over: game_over.cc
	g++ -Wall -o game_over game_over.cc $(LDFLAGS)
