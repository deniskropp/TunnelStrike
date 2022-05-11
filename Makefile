CXXFLAGS += -std=c++17

INCLUDES = \
	-Igeometry		\
	`pkg-config --cflags sfml-audio sfml-graphics sfml-system sfml-window`

OBJECTS = \
	Crosshair.o		\
	main.o			\
	Quad.o			\
	Sfx.o			\
	Shot.o			\
	Shots.o			\
	Target.o		\
	Targets.o		\
	Walls.o			\
	World.o			\
	geometry/camera3d.o	\
	geometry/geometry.o	\
	geometry/plane3d.o	\
	geometry/segment3d.o	\
	geometry/segmented.o	\
	geometry/solid3d.o	\
	geometry/vector3d.o	\
	utils/looptimer.o	\
	utils/parameters.o	\
	utils/tools.o

all: TunnelStrike

TunnelStrike: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $+ `pkg-config --libs sfml-audio sfml-graphics sfml-system sfml-window`

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $+

clean:
	rm -f TunnelStrike $(OBJECTS)
