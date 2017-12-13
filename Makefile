# The MIT License (MIT)

# Copyright (c) 2017 nabijaczleweli

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


include configMakefile


LDAR := $(PIC) $(LNCXXAR) -L$(OUTDIR) $(foreach l, ,-L$(BLDDIR)$(l)) $(foreach l,$(OS_LD_LIBS),-l$(l))
VERAR := $(foreach l,PB_CPP,-D$(l)_VERSION='$($(l)_VERSION)')
INCAR := $(foreach l,$(foreach l, ,ext/$(l)/include),-isystem$(l))
TEST_SOURCES := $(sort $(wildcard tests/*.cpp tests/**/*.cpp tests/**/**/*.cpp tests/**/**/**/*.cpp))
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))

.PHONY : all clean static dll tests run-tests

all : static dll tests run-tests

clean :
	rm -rf $(OUTDIR)

run-tests : $(OUTDIR)pb-cpp-tests$(EXE)
	$^

static : $(OUTDIR)libpb-cpp$(ARCH)
dll : $(OUTDIR)$(DLL_PREFIX)pb-cpp$(DLL)
tests : $(OUTDIR)pb-cpp-tests$(EXE)


$(OUTDIR)libpb-cpp$(ARCH) : $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%$(OBJ),$(SOURCES))
	$(AR) crs $@ $^

$(OUTDIR)$(DLL_PREFIX)pb-cpp$(DLL) : $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%$(OBJ),$(SOURCES))
	$(CXX) $(CXXAR) -shared -o$@ $^ $(PIC) $(LDAR)

$(OUTDIR)pb-cpp-tests$(EXE) : static $(patsubst tests/%.cpp,$(BLDDIR)test_obj/%$(OBJ),$(TEST_SOURCES)) $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%$(OBJ),$(SOURCES))
	$(CXX) $(CXXAR) -o$@ $(filter-out static,$^) $(PIC) $(LDAR) -lpb-cpp


$(OBJDIR)%$(OBJ) : $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) $(VERAR) -c -o$@ $^

$(BLDDIR)test_obj/%$(OBJ) : tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) -c -o$@ $^
