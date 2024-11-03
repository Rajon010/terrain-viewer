SHELL := /bin/bash
CXX := g++
WFLAGS := -Wall -Wextra -Wint-conversion -Warith-conversion -Wtraditional-conversion -Wconversion -Wenum-conversion -Wsign-conversion -Wfloat-conversion -Wdouble-promotion
CXXFLAGS := -O3 -std=c++23 -shared -fPIC
CPPFLAGS := $(shell python3 -m pybind11 --includes)
SO_SUFFIX := $(shell python3-config --extension-suffix)

.PHONY: all
all: ray_tracing_cpp$(SO_SUFFIX)

ray_tracing_cpp$(SO_SUFFIX): ray_tracing.cpp
	$(CXX) $(CPPFLAGS) $(WFLAGS) $(CXXFLAGS) -o $@ $^
