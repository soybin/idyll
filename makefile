CC = g++ -g
CCFLAGS = -pthread -o idyll -Isrc/lib

idyll: src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/renderer.cpp src/fractal.cpp src/seed.cpp src/lib/TinyPngOut.cpp
	$(CC) $(CCFLAGS) src/main.cpp src/config.cpp src/gui.cpp src/math.cpp src/renderer.cpp src/fractal.cpp src/seed.cpp src/lib/TinyPngOut.cpp

clean:
	rm idyll

.PHONY: all idyll clean
