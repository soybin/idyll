CC = g++ -g
CCFLAGS = -o idyll
LCFLAGS = -lncurses

idyll: src/main.cpp src/math.cpp src/region.cpp src/seed.cpp
	$(CC) $(CCFLAGS) src/main.cpp src/math.cpp src/region.cpp src/seed.cpp $(LCFLAGS)
	./idyll

clean:
	rm idyll

.PHONY: all idyll clean
