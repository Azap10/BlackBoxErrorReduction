CFLAGS := -lm -Wall

functionPatching: functionPatching.o
	gcc -o $@ $^ $(CFLAGS)

functionPatching.o: functionPatching.c
	gcc -c $^

clean:
	rm *.o
	rm *.ppm