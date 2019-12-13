all: demo-simulator demo-pine64 game-framework-simulator

demo-simulator:
	mkdir -p bin
	g++ -fpermissive -Wall -o ./bin/test-simulator src/example/*.cpp -lSDL2 -I./deps/Pine64-CPP/ -DSIMULATOR

demo-pine64:
	mkdir -p bin
	g++ -fpermissive -Wall -o ./bin/test src/example/*.cpp deps/Pine64-CPP/spi.cpp deps/Pine64-CPP/i2c.cpp deps/Pine64-CPP/gpio.cpp -I./deps/Pine64-CPP/ -DPINE64

game-framework-simulator:
	mkdir -p bin
	g++ -fpermissive -Wall -o ./bin/game-framework-simulator src/game-framework/*.cpp -lSDL2 -llua -I./deps/Pine64-CPP -I./deps/LuaBinder/files/src -DSIMULATOR




