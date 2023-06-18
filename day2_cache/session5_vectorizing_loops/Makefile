CXX=g++
CXXFLAGS=-O3 -msse2  -fopt-info-vec-all # -fopt-info-vec-optimized 
#-ftree-vectorizer-verbose=1 old
#CXX=icpc
#CXXFLAGS=-fast -vec-report3
LDFLAGS= -l papi

all: vector_add vector_dot vector_saxpy

vector_add:	vector_add_main.o simple_papi.o vector_add.o vector_base.o
	$(CXX) -o $@ vector_add_main.o simple_papi.o vector_add.o vector_base.o  $(LDFLAGS) 

vector_dot:	vector_dot_main.o simple_papi.o  vector_dot.o vector_base.o 
	$(CXX) -o $@ vector_dot_main.o  simple_papi.o vector_dot.o vector_base.o  $(LDFLAGS)

vector_saxpy:	vector_saxpy_main.o simple_papi.o saxpy.o vector_base.o
	$(CXX) -o $@ vector_saxpy_main.o saxpy.o simple_papi.o vector_base.o  $(LDFLAGS)  


.SUFFIXES:	.o	.cpp

vector_dot_main.o vector_dot.o: vector_dot.h
vector_add_main.o vector_add.o: vector_add.h
vector_saxpy_main.o saxpy.o: saxpy.h

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:	
	rm -f *.o  vector_add vector_dot vector_saxpy *~ make_output
