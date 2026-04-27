CXX = g++
CXXFLAGS = -std=c++17 -pthread


objects = main.o factorial.o matrix.o

res: $(objects)
	$(CXX) -o res $(objects)

main.o: factorial.h matrix.h ThreadPool.h
factorial.o: factorial.h
matrix.o: matrix.h

.PHONY: clean
clean: 
	rm res $(objects)