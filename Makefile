all:
	clear
	g++-11 main.cpp ping.cpp -o program

run: all
	./program