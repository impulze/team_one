CXX = clang++ -std=c++11

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

LDLIBS += -lsqlite3

OBJS = Database.o SQLiteDatabase.o
OBJS += ClientCollection.o Client.o MainNetworkEventHandler.o
OBJS += Message.o NetworkInterface.o
OBJS += cte_server.o

all: Server

Server: $(OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)

clean:
	$(RM) Server $(OBJS)
