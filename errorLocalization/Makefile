CFLAGS := -lm -Wall

all: mpfrErrorLocalization errorLocalization

mpfrErrorLocalization: mpfrErrorLocalization.o
	gcc -o $@ $^ $(CFLAGS) -lmpfr

mpfrErrorLocalization.o: mpfrErrorLocalization.c 
	gcc -c $^

errorLocalization: errorLocalization.o 
	gcc -o $@ $^ $(CFLAGS)

errorLocalization.o: errorLocalization.c
	gcc -c $^	

clean:
	rm *.o
	rm *.ppm
