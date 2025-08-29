CXX = g++
CXXFLAGS = -Wall -g `xml2-config --cflags`
LDFLAGS = `xml2-config --libs`
TARGETS = attributor

all: $(TARGETS)

attributor: attributor.cpp
	$(CXX) $(CXXFLAGS) -o attributor attributor.cpp openxml.cpp -lcrypto $(LDFLAGS)

tests:
	g++ -Wall -g -o test1 test1.cpp
	g++ -Wall -g -o test2 test2.cpp
	g++ -Wall -g -o test3 test3.cpp
	g++ -Wall -g -o test4 test4.cpp
	g++ -Wall -g -o test5 test5.cpp


clean:
	rm -f $(TARGETS)