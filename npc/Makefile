ifeq ($(BLOCK),)
$(error need a BLOCK)
endif

CSRC=$(abspath $(wildcard csrc/$(BLOCK)/*.cpp))
VSRC=$(abspath $(wildcard vsrc/$(BLOCK)/*.v vsrc/$(BLOCK)/**/*.v))

TRACE ?= --trace-fst
TOP   ?= top

.PHONY: sim
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(TRACE) -Wall  --cc  --build --exe --top $(TOP) --Mdir  ./build/$(BLOCK) $(CSRC) $(VSRC)

.PHONY: clean
clean:
	rm -rf ./obj_dir/

include ../Makefile
