SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
BINARY ?= Codec.exe

NDEBUG ?= -DNDEBUG
MEASURE_TIME ?= -DMEASURE_TIME
EXTRA_DEFINES ?=
DEFINES := $(NDEBUG) $(MEASURE_TIME) $(EXTRA_DEFINES)

LTO ?= -flto # run "LTO='' make" if -flto cause error
CXXFLAGS ?= -std=c++0x -Wall -Wextra -Werror -pedantic -O3 $(LTO) -g $(DEFINES)
LDFLAGS ?= $(LTO) -g
LIBS := -lz

MAKEDEPENDSFLAG ?= -MM
MAKEDEPENDS ?= $(CXX) $(CXXFLAGS) $(MAKEDEPENDSFLAG)
DEPSDIR ?= .deps

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

-include $(wildcard $(DEPSDIR)/*.d)

$(DEPSDIR):
	mkdir $@

%.o: %.cpp $(DEPSDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
ifneq ($(MAKEDEPENDS),)
	$(MAKEDEPENDS) $< > $(DEPSDIR)/$(@:.o=.d)
endif

clean:
	rm -rf $(DEPSDIR) $(OBJECTS) $(BINARY)
