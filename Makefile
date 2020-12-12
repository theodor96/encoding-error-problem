all: main

main: main.cpp
	@echo "Building executable..."
	@g++ -std=c++17 -o main main.cpp

clean:
	rm -rf main
