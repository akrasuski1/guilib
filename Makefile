

all:
	g++ main.cpp visualization.cpp -lsfml-graphics -lsfml-window -lsfml-system -pthread -std=c++14 -O2 -Wno-unused-result -o run
