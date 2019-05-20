all: main

main: main.cpp BPlusTree.cpp
	g++ -std=c++11 -o main main.cpp BPlusTree.cpp -lstdc++fs
clean:
	rm main
