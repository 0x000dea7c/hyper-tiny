SYSCONF_LINK = g++
CPPFLAGS     = -Wall -Wextra -pedantic -std=c++17 -pg -O3 -ggdb3 -march=native
LDFLAGS      =
LIBS         = -lm -pg

DESTDIR = ./
TARGET  = main

OBJECTS := $(patsubst %.cc,%.o,$(wildcard *.cc))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cc
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga
	-rm -f gmon.out
