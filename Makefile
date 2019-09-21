## Base compiler options
CXXFLAGS := -std=c++17 -MMD

## Libraries to link in
LIBS := pthread

## Prefer clang over gcc if available
ifneq ($(shell which clang++ 2>/dev/null),)
CXX := clang++
endif

## Diagnostic options
CXXFLAGS += -Wall -Wextra -Werror
# CXXFLAGS += -Og
# CXXFLAGS += -fsanitize=address
# CXXFLAGS += -fsanitize=thread
# CXXFLAGS += -fsanitize=undef

## Production options
CXXFLAGS += -O2
CXXFLAGS += -flto
CXXFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += --gc-sections

## Phony targets

.PHONY: all clean test

all: example

clean:
	rm -f -- *.oxx *.d example test.dat

test: test-order test-nocopy

test-nocopy:
	$(CXX) $(CXXFLAGS) -c -fsyntax-only NoCopy.cpp

test-order: example test.dat
	@echo ""
	@echo "Serial / single-worker test for output ordering"
	@time WORKERS=1 ./example < test.dat | sort -cn
	@echo ""
	@echo "Parallel / multiple-worker test for output ordering"
	@time WORKERS=64 ./example < test.dat | sort -cn
	@echo ""

## File recipes

example: Example.oxx Pipeline.oxx
	$(CXX) $(CXXFLAGS) $(LDFLAGS:%=-Wl,%) -o $@ $^ $(LIBS:%=-l%)

%.oxx: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


test.dat:
	for x in {1..100000}; do \
		echo "$$x 10"; \
	done > $@

## Compiler-generated dependency graph

-include: $(wildcard *.d)
