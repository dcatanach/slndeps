
all: slndeps

slndeps: slndeps.o
	$(CXX) -o slndeps slndeps.o


