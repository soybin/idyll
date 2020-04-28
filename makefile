CC = g++ -g
CCFLAGS = -pthread -o idyll
LCFLAGS = -lncurses

idyll: src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/region.cpp src/fractal.cpp src/seed.cpp
	$(CC) $(CCFLAGS) src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/region.cpp src/fractal.cpp src/seed.cpp $(LCFLAGS)
	./idyll
	feh

clean:
	rm idyll

.PHONY: all idyll clean
