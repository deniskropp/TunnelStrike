all: TunnelStrike

CXXFLAGS += \
	-std=c++17	\
	-O2 -g2	\
	-MMD -MP

CPPFLAGS = \
	-Isrc	\
	-Isrc/geometry	\
	`pkg-config --cflags sfml-system`

LIBS = \
	`pkg-config --libs sfml-audio sfml-graphics sfml-system sfml-window`

GAME_OBJECTS = \
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
	objs/evo/Population.o		\
	objs/persist/Archive.o		\
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

SIM_OBJECTS = \
	$(filter-out objs/main.o,$(GAME_OBJECTS)) \
	objs/sim_headless.o

TunnelStrike: $(GAME_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

TunnelStrike-sim: $(SIM_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LIBS)

sim: TunnelStrike-sim

objs/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

-include $(GAME_OBJECTS:.o=.d)
-include objs/sim_headless.d

clean:
	$(RM) TunnelStrike TunnelStrike-sim $(GAME_OBJECTS) objs/sim_headless.o $(GAME_OBJECTS:.o=.d) objs/sim_headless.d
