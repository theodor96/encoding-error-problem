all: main

main: main.cpp
	@echo "Building executable..."
	@g++ -Werror -Wall -Wextra -Wpedantic -Wfloat-equal -Wctor-dtor-privacy -Winit-self -Wmissing-declarations -Wnoexcept -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wundef -Wno-missing-declarations -std=c++17 -o main main.cpp

clean:
	rm -rf main
