all:
	gcc imHere.c -o imHere -lpthread
	gcc whoIsThere.c -o whoIsThere -lpthread 
arm:
	arm-hisiv100nptl-linux-gcc imHere.c -o imHere -lpthread
	arm-hisiv100nptl-linux-gcc whoIsThere.c -o whoIsThere -lpthread
x86-lib:
	gcc -c imHere.c
	gcc -c whoIsThere.c
	ar -cr libscan.a *.o
arm-lib:
	arm-hisiv100nptl-linux-gcc -c imHere.c
	arm-hisiv100nptl-linux-gcc -c whoIsThere.c
	arm-hisiv100nptl-linux-ar -cr libscan.a *.o
clean:
	rm *.o
	rm *.a
	rm imHere
	rm whoIsThere
