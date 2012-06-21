CXX = clang++ -std=c++11
VALGRIND = valgrind

CXXFLAGS += -Wall -Wextra
CXXFLAGS += $(shell ncursesw5-config --cflags)
CXXFLAGS += $(shell pkg-config --cflags openssl)
LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

LDLIBS += -lsqlite3
LDLIBS += $(shell ncursesw5-config --libs)
LDLIBS += $(shell pkg-config --libs openssl)

OBJS = Database.o SQLiteDatabase.o
OBJS += CommandProcessor.o Hash.o
OBJS += ClientCollection.o Client.o
OBJS += Message.o NetworkInterface.o
OBJS += UserInterface.o NCursesUserInterface.o
OBJS += Document.o UserDatabase.o
OBJS += main_network_message_handler.o

TEST_OBJS += tests/Database.o tests/SQLiteDatabase.o tests/cte_server.o

BIN_OBJS = $(OBJS) cte_server.o
BIN_SRCS = $(BIN_OBJS:%.o=%.cpp)
BIN_DEPS = $(BIN_OBJS:%=deps/%)

TEST_BIN_OBJS = $(OBJS) $(TEST_OBJS) tests/cte_server.o
TEST_BIN_SRCS = $(TEST_BIN_OBJS:%.o=%.cpp)
TEST_BIN_DEPS = $(TEST_BIN_OBJS:%=deps/%)

all: Server tests/Server

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -MM -MT $@ $< > deps/$@
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -o $@ -c $<

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

valgrind:
	$(VALGRIND) \
		--leak-check=full \
		--show-reachable=yes \
		--suppressions=ncurses.supp \
		./Server

-include $(BIN_DEPS)
-include $(TEST_BIN_DEPS)
-include config.mk
