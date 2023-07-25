CC:= gcc
CFLAGS:= -O3 -D_GNU_SOURCE -D_POSIX_SOURCE -m64 -falign-functions=64 -Wall
LIBS:= -lpthread -lm
UTILS:= ../util/util.o 
all: obj bin out data driver

driver: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS)

driver-input: obj/driver.o $(UTILS)
	$(CC) -o bin/$@ $^ $(LIBS)

obj/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

obj:
	mkdir -p $@

bin:
	mkdir -p $@

out:
	mkdir -p $@

data:
	mkdir -p $@

clean:
	rm -rf bin obj out
	rm -rf ../util/*.o

.PHONY: all clean