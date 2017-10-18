
FILES = `find src -name '*.cpp'`

default: build run

build:
	g++ -o out/prog -std=c++11 $(FILES)

run:
	out/prog
