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
BIN_DEPS = $(BIN_OBJS:%=deps/%)

TEST_BIN_OBJS = $(OBJS) $(TEST_OBJS) tests/cte_server.o
TEST_BIN_SRCS = $(TEST_BIN_OBJS:%.o=%.cpp)
TEST_BIN_DEPS = $(TEST_BIN_OBJS:%=deps/%)

all: Server tests/Server

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -MM -MT $@ $^ > deps/$@
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -o $@ -c $^

Server: $(BIN_OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)

tests/%.o: CXXFLAGS += $(BOOST_UTF_CXXFLAGS) -I./
tests/Server: $(TEST_BIN_OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDLIBS)

depend: $(BIN_DEPS) $(TEST_BIN_DEPS)

clean:
	$(RM) Server $(BIN_OBJS)
	$(RM) tests/Server $(TEST_BIN_OBJS)
	$(RM) $(BIN_DEPS)
	$(RM) $(TEST_BIN_DEPS)

-include $(BIN_DEPS)
-include $(TEST_BIN_DEPS)
