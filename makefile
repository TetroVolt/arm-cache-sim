
FILES = `find src -name '*.cpp'`

default: build run

build:
	g++ -o out/prog -std=c++11 $(FILES)

debug:
	g++ -g -o out/prog -std=c++11 $(FILES)
	gdb --args out/prog -f tracefile.trace

run:
	out/prog tracefile.trace

