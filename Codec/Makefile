SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
BINARY ?= Codec.exe
LIBRARYSOURCES := $(wildcard src/lib/*.cpp)
LIBRARYOBJECTS := $(LIBRARYSOURCES:.cpp=.o)
LIBRARY ?= libCodec.a
TESTSOURCES := $(wildcard test/*.cpp)
TESTOBJECTS := $(TESTSOURCES:.cpp=.o)
TESTBINARY ?= CodecTest.exe

NDEBUG ?= -DNDEBUG
MEASURETIME ?= -DMEASURE_TIME
EXTRADEFINES ?=
DEFINES := $(NDEBUG) $(MEASURETIME) $(EXTRADEFINES)

LTO ?= -flto # run "LTO='' make" if -flto cause error
CXXFLAGS ?= -std=c++0x -Wall -Wextra -Werror -pedantic -O3 $(LTO) -g $(DEFINES)
LDFLAGS ?= $(LTO) -g
LIBS := -lz

MAKEDEPENDSFLAG ?= -MM
MAKEDEPENDSTARGETFLAG ?= -MT
MAKEDEPENDS ?= $(CXX) $(CXXFLAGS) $(MAKEDEPENDSFLAG)
DEPSDIR ?= .deps

.PHONY: all clean test

all: $(BINARY) $(LIBRARY) $(TESTBINARY)

$(BINARY): $(OBJECTS) $(LIBRARY)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(LIBRARY): $(LIBRARYOBJECTS)
	$(AR) -cr $@ $^

$(TESTBINARY): $(TESTOBJECTS) $(LIBRARY)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

-include $(wildcard $(DEPSDIR)/*.d)

$(DEPSDIR):
	mkdir $@

%.o: %.cpp | $(DEPSDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
ifneq ($(MAKEDEPENDS),)
ifneq ($(MAKEDEPENDSTARGETFLAG),)
	$(MAKEDEPENDS) $(MAKEDEPENDSTARGETFLAG) $@ $< > $(DEPSDIR)/$(subst /,_,$(@:.o=.d))
else
	$(MAKEDEPENDS) $< > $(DEPSDIR)/$(subst /,_,$(@:.o=.d))
endif
endif

clean:
	rm -rf $(DEPSDIR) $(OBJECTS) $(BINARY) $(LIBRARYOBJECTS) $(LIBRARY) \
	$(TESTBINARY) $(TESTOBJECTS)

test: $(TESTBINARY)
	$(TESTBINARY)
