all: TunnelStrike

CXXFLAGS += \
	-std=c++17	\
	-O2 -g2

CPPFLAGS = \
	-Igeometry	\
	`pkg-config --cflags sfml-system`

LIBS = \
	`pkg-config --libs sfml-audio sfml-graphics sfml-system sfml-window`

OBJECTS = \
	objs/Crosshair.o			\
	objs/main.o					\
	objs/Quad.o					\
	objs/Sfx.o					\
	objs/Shot.o					\
	objs/Shots.o				\
	objs/Target.o				\
	objs/Targets.o				\
	objs/Walls.o				\
	objs/World.o				\
	objs/geometry/camera3d.o	\
	objs/geometry/geometry.o	\
	objs/geometry/plane3d.o		\
	objs/geometry/segment3d.o	\
	objs/geometry/segmented.o	\
	objs/geometry/solid3d.o		\
	objs/geometry/vector3d.o	\
	objs/utils/looptimer.o		\
	objs/utils/parameters.o		\
	objs/utils/tools.o

TunnelStrike: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

objs/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $+

clean:
	$(RM) TunnelStrike $(OBJECTS)
