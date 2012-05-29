CXX = clang++ -std=c++11

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

LDLIBS += -lsqlite3

OBJS = Database.o SQLiteDatabase.o Server.o

all: Server

Server: $(OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)

clean:
	$(RM) Server $(OBJS)
