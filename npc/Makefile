ifeq ($(BLOCK),)
$(error need a BLOCK)
endif

INC_PATH = $(abspath csrc/$(BLOCK)/include)

CSRC=$(abspath $(shell find csrc/$(BLOCK)/src -name "*.cpp"))
VSRC=$(abspath $(shell find vsrc/$(BLOCK)/ -name "*.v"))

TRACE ?= --trace-fst
TOP   ?= top

CXXFLAGS += $(addprefix -I, $(INC_PATH))
LDFLAGS  += -L${NPC_HOME}/build/
LDLIBS   += -lreadline -ldisasm
.PHONY: sim
sim: 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(TRACE) -Wall  --cc $(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS) $(LDLIBS)) \
		                 --build --exe --top $(TOP) --Mdir ./build/$(BLOCK) $(CSRC) $(VSRC)

run:
	echo V$(TOP) $(IMG)
	./build/$(BLOCK)/V$(TOP) $(IMG)

.PHONY: clean
clean:
	rm -rf ./build/$(BLOCK)

include ../Makefile
