all: compile link

compile:
	g++ -I include -c src/search100.cpp

link:
	g++ search100.o -o search100 -L lib -l sfml-graphics -l sfml-window -l sfml-system -l opengl32 -l sfml-audio
