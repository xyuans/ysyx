ifeq ($(BLOCK),)
$(error need a BLOCK)
endif

CSRC=$(abspath $(wildcard csrc/$(BLOCK)/*.cpp) vsrc/$(BLOCK)/**/.cpp)
VSRC=$(abspath $(wildcard vsrc/$(BLOCK)/*.v vsrc/$(BLOCK)/**/*.v))

TRACE ?= --trace-fst
TOP   ?= top

.PHONY: sim
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(TRACE) -Wall  --cc  --build --exe --top $(TOP) --Mdir  ./build/$(BLOCK) $(CSRC) $(VSRC)

run:
	echo V$(TOP) $(IMG).bin
	./build/$(BLOCK)/V$(TOP) $(IMG)

.PHONY: clean
clean:
	rm -rf ./obj_dir/

include ../Makefile
