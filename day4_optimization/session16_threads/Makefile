default: race bufferMon bufferSem  hello1  hello2  hello  philosophers

.SUFFIXES:
%.o: %.cpp
	 g++ -std=c++11 -pthread -c $*.cpp
%: %.o
	g++ -std=c++11 -pthread -o $@ $*.o

clean:
	rm -f race bufferMon bufferSem  hello1  hello2  hello  philosophers

