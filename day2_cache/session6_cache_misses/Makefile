CXXFLAGS=-O3
CXFALIAS=-O1
CXX=gcc
BASE=papiHighLevel
BASE_O1=$(BASE)_O1
TARGET=$(BASE) $(BASE_O1)
#LDFLAGS=-L $(LIBRARY_PATH) 
LIBS=-lpapi
STD=-std=c99

all:	$(TARGET)	

$(BASE):	$(BASE).c 
	$(CXX) $(CXXFLAGS) $(BASE).c -o $@ $(STD) $(LDFLAGS) $(LIBS) -lm 

$(BASE_O1):	$(BASE).c
	$(CXX) $(CXFALIAS) $(BASE).c -o $@ $(STD) $(LDFLAGS) $(LIBS) -lm 

clean:	
	rm -f $(TARGET)
