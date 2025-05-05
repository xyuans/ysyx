VSOURCES=$(wildcard vsrc/*.v vsrc/**/*.v)

all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@verlator -Wall --trace --cc --build --exe sim_main.cpp
include ../Makefile
