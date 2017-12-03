
FILES = `find src -name '*.cpp'`

build:
	g++ -o ./prog -std=c++11 $(FILES)

debug:
	g++ -g -o ./prog -std=c++11 $(FILES)
	gdb --args ./prog -f tracefile.trace

run:
	echo "" > ./test_results.txt
	echo -e "\ntrace1" >> test_results.txt
	./prog trace1.trace >> test_results.txt
	echo -e"\ntrace2" >> test_results.txt
	./prog trace2.trace >> test_results.txt
	echo -e "\ntrace3" >> test_results.txt
	./prog trace3.trace >> test_results.txt
	echo -e "\ntrace4" >> test_results.txt
	./prog trace4.trace >> test_results.txt
	echo -e "\ntrace5" >> test_results.txt
	./prog trace5.trace >> test_results.txt

