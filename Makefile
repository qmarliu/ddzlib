#rocenting@gmail.com
CC = gcc
CXX = g++
CFLAGS = -g -O2 -Wall 
CXXFLAGS = -g -O2 -frtti -fexceptions -Wall -std=c++11
INCPATH = #-I/usr/include/mysql
LD = gcc
LFLAGS = #-static
LIBS = -L. -lstdc++ -lpthread

STATIC_LIB = libddz.a
OBJECTS_LIB = card.o deck.o hole_cards.o community_cards.o card_statistics.o card_analysis.o card_find.o libgtest.a

DESTDIR_TARGET = test
OBJECTS = test_lib.o $(OBJECTS_LIB)

all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

lib: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS_LIB)
	ar -r $(STATIC_LIB) $(OBJECTS_LIB)

####### Implicit rules ######

.SUFFIXES: .cpp .cc .cxx .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

clean:
	rm -rf $(DESTDIR_TARGET) *.o
