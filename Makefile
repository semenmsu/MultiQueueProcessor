all: clean mkdir main test1 test2 test3 test4

mkdir:
	mkdir -p bin/

main:
	g++ -std=c++17 -O2 -o bin/main main.cpp -lpthread

test1:
	g++ -std=c++17 -O2 -o bin/test1 test1.cpp -lpthread

test2:
	g++ -std=c++17 -O2 -o bin/test2 test2.cpp -lpthread

test3:
	g++ -std=c++17 -O2 -o bin/test3 test3.cpp -lpthread

test4:
	g++ -std=c++17 -O2 -o bin/test4 test4.cpp -lpthread


clean:
	rm -rf bin/*