# This optimization is neccessary for inline expansion
# of functions
CXXFLAGS="-O1"

all: binary dataparser 

binary: main.o
	    g++ main.o -o main

dataparser: dataparse.cpp
	    g++ dataparse.cpp -o dataparser

main.o: main.cpp
	    g++ -c main.cpp

dascontrol: dasmanipulator.cpp
		g++ dasmanipulator.cpp vcmdas.cpp -o dascontrol

pwmset: pwmset.cpp
		g++ pwmset.cpp python_io.cpp -o pwmset

pwmloop: pwmloop.cpp
		g++ -static pwmloop.cpp python_io.cpp vcmdas.cpp -o pwmloop

permissions:
		chown root:root main
		chmod +s main

clean:
	    rm -rf *o *.gch main dataparser
