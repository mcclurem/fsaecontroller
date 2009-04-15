# This optimization is neccessary for inline expansion
# of functions
CXXFLAGS="-O1"

all: binary dataparser 

binary: python.o vcmdas.o table.o car.o
	    g++ python.o vcmdas.o table.o car.o main.cpp -o main

dataparser:
	    g++ dataparse.cpp -o dataparser

dascontrol: vcmdas.o
		g++ dasmanipulator.cpp vcmdas.o -o dascontrol

pwmset:
		g++ pwmset.cpp python_io.o -o pwmset

pwmloop: python.o
		g++ -static pwmloop.cpp python_io.o vcmdas.o -o pwmloop

tabletest: table.o
		g++ -static tabletest.cpp table.cpp -o tabletest

python.o:
		g++ -c python_io.cpp -o python.o

vcmdas.o:
		g++ -c vcmdas.cpp

car.o:
		g++ -c car.cpp

table.o:
		g++ -c table.cpp

permissions:
		chown root:root main
		chmod +s main

clean:
	    rm -rf *o *.gch main dataparser
