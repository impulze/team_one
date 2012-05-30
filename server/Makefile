CXX = clang++ -std=c++11

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

BOOST_UTF_CXXFLAGS = -I/usr/include/boost/

LDLIBS += -lsqlite3

OBJS = Database.o SQLiteDatabase.o
OBJS += ClientCollection.o Client.o MainNetworkEventHandler.o
OBJS += Message.o NetworkInterface.o

TEST_OBJS += tests/SQLiteDatabase.o tests/cte_server.o

BIN_OBJS = $(OBJS) cte_server.o
BIN_SRCS = $(BIN_OBJS:%.o=%.cpp)

TEST_BIN_OBJS = $(OBJS) $(TEST_OBJS) tests/cte_server.o
TEST_BIN_SRCS = $(TEST_BIN_OBJS:%.o=%.cpp)

all: Server tests/Server

Server: $(BIN_OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)

tests/%.o: CXXFLAGS += $(BOOST_UTF_CXXFLAGS) -I./
tests/Server: $(TEST_BIN_OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)


clean:
	$(RM) Server $(BIN_OBJS)
	$(RM) tests/Server $(TEST_BIN_OBJS)

Makefile.depend: $(BIN_SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ > ./Makefile.depend

#Makefile.depend: $(OBJS:%.o=%.cpp)
#	$(CXX) -MM $^ > ./Makefile.depend

depend: ./Makefile.depend ./Makefile.depend.tests

#-include ./Makefile.depend
