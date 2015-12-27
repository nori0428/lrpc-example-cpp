LRPC_ROOT = /Users/kobota/work/linear-cpp
CFLAGS = -Wall -I$(LRPC_ROOT)/include -I$(LRPC_ROOT)/deps/msgpack/include -I$(LRPC_ROOT)/deps/ -std=c++11
LDFLAGS = -L$(LRPC_ROOT)/src/.libs -llinear -ltv -luv -L$(LRPC_ROOT)/deps/libtv/src/.libs -L$(LRPC_ROOT)/deps/libtv/deps/libuv/.libs

.SUFFIXES: .cc .o

all : client server

client : client.o
	g++ -o client client.o $(LDFLAGS)

server : server.o
	g++ -o server server.o $(LDFLAGS)

.cc.o :
	g++ -c $(CFLAGS) -I. $< -o $@

clean :
	rm -f *.o server client
