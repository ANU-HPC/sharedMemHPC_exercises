CXX=gcc
CXXFLAGS=-O3
PROGS=papiSimple papiHighLevel 
#LDFLAGS=-L $(LIBRARY_PATH) 
LIBS=-lpapi
STD=-std=c99

all: $(PROGS)

%: %.c
	$(CXX) $< -o $@  $(LDFLAGS) $(LIBS) -lm

%.o : %.c
	$(CXX) $(CXXFLAGS) -c $< $(STD)

clean:	
	rm -f *.o $(PROGS)
