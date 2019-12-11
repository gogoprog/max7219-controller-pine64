simulator:
	mkdir -p bin
	g++ -fpermissive -Wall -o ./bin/test-simulator src/example/*.cpp -lSDL

pine64:
	mkdir -p bin
	g++ -fpermissive -Wall -o ./bin/test src/example/*.cpp deps/Pine64-CPP/spi.cpp deps/Pine64-CPP/i2c.cpp deps/Pine64-CPP/gpio.cpp -I./deps/Pine64-CPP/ -DPINE64



