# This optimization is neccessary for inline expansion
# of functions
CXXFLAGS=-O1

all: binary 

binary: vcmdas.o table.o car.o pcm.o
	g++ $(CXXFLAGS) -static pcm.o vcmdas.o table.o car.o main.cpp -o main

inputtest: python.o vcmdas.o
	g++ $(CXXFLAGS) -static python.o vcmdas.o inputtest.cpp -o inputtest

outputtest: python.o vcmdas.o
	g++ -static python.o vcmdas.o outputtest.cpp -o outputtest

modeselect: pcm.o
	g++ $(CXXFLAGS) -static pcm.o modeselect.cpp -o modeselect
	


dataparser:
	g++ -static dataparse.cpp -o dataparser

dascontrol: vcmdas.o
	g++ -static dasmanipulator.cpp vcmdas.o -o dascontrol

pwmset: python.o
	g++ -static pwmset.cpp python.o -o pwmset

pwmloop: python.o vcmdas.o
	g++ -static pwmloop.cpp python.o vcmdas.o -o pwmloop

tabletest: table.o
	g++ -static tabletest.cpp table.cpp -o tabletest

python.o:
	g++ -c python_io.cpp -o python.o

pcm.o:
	g++ -c pcm.cpp -o pcm.o

vcmdas.o:
	g++ -c vcmdas.cpp -o vcmdas.o

car.o:
	g++ -c car.cpp -o car.o

table.o:
	g++ -c table.cpp -o table.o

permissions:
	chown root:root main
	chmod +s main

clean:
	rm -rf *o *.gch main dataparser inputtest outputtest dascontrol pwmloop pwmset tabletest
