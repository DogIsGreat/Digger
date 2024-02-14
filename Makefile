#Header components, Flags, Sources, Libraries, and Locations to be compiled.
CC=clang 
###Extra c flags
CFLAGS=-g -O2  -Wall -Wextra -Isrc  $(OPTFLAGS)
INCLUDES=-I/opt/homebrew/opt/unixodbc/include
LDLIBS=-L/opt/homebrew/opt/unixodbc/lib $(OPTLIBS)
LIBS= -lodbc $(OPTLIBS)

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

###Change TARGET
TARGET=build/Digger
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

###Change EXECUTABLE
EXECUTABLE=bin/digger

#Target Build Components and Flag options
.PHONY: all dev clean install
all: $(TARGET) $(SO_TARGET) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJECTS)  $(LDLIBS) $(LIBS)
	chmod +x $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

dev: CFLAGS=-g -Wall  -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS) 
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(INCLUDES) $(LDLIBS) $(LIBS)

build:
	@mkdir -p build
	@mkdir -p bin

#Cleaning 
clean:
	rm -rf build $(OBJECTS) $(TESTS) $(EXECUTABLE) 
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

#Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/