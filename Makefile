all:
	gcc imHere.c -o imHere -lpthread
	gcc whoIsThere.c -o whoIsThere -lpthread
clean:
	rm imHere
	rm whoIsThere
