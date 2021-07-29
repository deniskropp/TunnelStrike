all: TunnelStrike

TunnelStrike: Crosshair.o main.o Quad.o Sfx.o Shot.o Shots.o Target.o Targets.o Walls.o World.o
	g++ -oTunnelStrike $+ `pkg-config --libs sfml-audio sfml-graphics`

%.o: %.cpp
	g++ -c -o$@ $+
