all: TunnelStrike

CXXFLAGS += \
	-std=c++17	\
	-O2 -g2

INCLUDES = \
	-Igeometry	\
	`pkg-config --cflags sfml-audio sfml-graphics sfml-system sfml-window`

LIBS = \
	`pkg-config --libs sfml-audio sfml-graphics sfml-system sfml-window`

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

TunnelStrike: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $+
