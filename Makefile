# -D_GLIBCXX_DEBUG -DDEBUG
SYSCONF_LINK := g++
CXXFLAGS     := -Wall -Wextra -pedantic -std=c++17 -pg -O0 -ggdb3 -march=native -Wformat -Wformat-security
LDFLAGS      :=
LIBS         := -lm -pg

DESTDIR = ./
TARGET  = main

OBJECTS := $(patsubst %.cc,%.o,$(wildcard *.cc))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cc
	$(SYSCONF_LINK) -Wall $(CXXFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga
	-rm -f gmon.out
