all: pool.o main.o
	g++ -o all pool.o main.o

main.o: main.cpp pool.hpp
	g++ -c main.cpp
pool.o: pool.cpp pool.hpp
	g++ -c pool.cpp

clean:
	rm all main.o
