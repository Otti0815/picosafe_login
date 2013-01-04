CC=gcc
OPTIMIZATION=-O2
CFLAGS=-Wall -Wstrict-prototypes
LDFLAGS=-lusb -pthread -fno-strict-aliasing -g -fwrapv -I `python-config --includes` -fPIC

all:
	python setup.py build

install:
	sudo python setup.py install

self:
	$(CC) $(LDFLAGS) $(CFLAGS) $(OPTIMIZATION) -c libpicosafelogin-py.c -o libpicosafelogin-py.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(OPTIMIZATION) -c libpicosafelogin.c -o libpicosafelogin.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(OPTIMIZATION) -c base64.c -o base64.o
	$(CC) $(LDFLAGS) $(CFLAGS) $(OPTIMIZATION) -c rijndael.c -o rijndael.o
	$(CC) -lusb -pthread -shared -Wl,-O2 -Wl,-Bsymbolic-functions $(OPTIMIZATION) libpicosafelogin-py.o base64.o rijndael.o libpicosafelogin.o -o picosafelogin.so

clean:
	rm -f *.o
	rm -rf build/
