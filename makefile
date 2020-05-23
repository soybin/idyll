CC = g++ -g
CCFLAGS = -pthread -o idyll
LCFLAGS = -lncurses

idyll: src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/renderer.cpp src/fractal.cpp src/seed.cpp src/TinyPngOut.cpp
	$(CC) $(CCFLAGS) src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/renderer.cpp src/fractal.cpp src/seed.cpp src/TinyPngOut.cpp $(LCFLAGS)
	./idyll
	feh render.png

clean:
	rm idyll

.PHONY: all idyll clean
