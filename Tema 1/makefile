all: matrix.o main.o
	g++ -o all matrix.o main.o

main.o: main.cpp matrix.hpp
	g++ -c -std=c++17 main.cpp
matrix.o: matrix.cpp matrix.hpp
	g++ -c -std=c++17 matrix.cpp

clean:
	rm all main.o
