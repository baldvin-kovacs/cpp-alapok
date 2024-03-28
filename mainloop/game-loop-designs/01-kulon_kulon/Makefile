all: nev_bekero

.PHONY: all

LDFLAGS = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

nev_bekero: nev_bekero.cc
	g++ -Wall -o nev_bekero nev_bekero.cc $(LDFLAGS)