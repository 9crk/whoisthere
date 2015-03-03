all:
	gcc imHere.c -o imHere
	gcc whoIsThere.c -o whoIsThere -lpthread
clean:
	rm imHere
	rm whoIsThere
