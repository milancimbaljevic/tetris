build:
	g++ tetris.cpp shape.cpp -pthread -lSDL2 -o tetris
	./tetris

clean:
	rm tetris
