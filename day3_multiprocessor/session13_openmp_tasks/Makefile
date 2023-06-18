CXX=g++
CXXFLAGS=-fopenmp -O3
#CXX=icx
#CXXFLAGS=-fast -vec-report3
LDFLAGS=-fopenmp

all: fibonacci vector_add vector_search

vector_add.o vector_add_main.o: vector_add.h

fibonacci:	fibonacci.o  
	$(CXX) -o $@ fibonacci.o   $(LDFLAGS) 

vector_add:	vector_add_main.o  vector_add.o  vector_base.o
	$(CXX) -o $@ vector_add_main.o  vector_add.o  vector_base.o $(LDFLAGS) 


vector_search:	vector_search_main.o  vector_search.o vector_base.o
	$(CXX) -o $@ vector_search_main.o vector_add.o  vector_search.o vector_base.o  $(LDFLAGS) 

#fibonacci:	fibonacci.o  vector_add.o vector_search.o vector_base.o
#	$(CXX) -o $@ main.o vector_add.o  vector_search.o vector_base.o  $(LDFLAGS) 

.SUFFIXES:	.o	.cpp

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<


clean:	
	rm -f *.o  fibonacci vector_add vector_search *~
