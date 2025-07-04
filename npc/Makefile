ifeq ($(BLOCK),)
$(error need a BLOCK)
endif

INC_PATH = $(abspath csrc/$(BLOCK)/include)

CSRC=$(abspath $(wildcard csrc/$(BLOCK)/*.cpp) csrc/$(BLOCK)/**/*.cpp)
VSRC=$(abspath $(wildcard vsrc/$(BLOCK)/*.v vsrc/$(BLOCK)/**/*.v))

TRACE ?= --trace-fst
TOP   ?= top

CXXFLAGS += $(addprefix -I, $(INC_PATH))
LDFLAGS  += -L${NEMU_HOME}/tools/capstone/repo/
LDLIBS   += -lreadline -l:libcapstone.so.5
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
	rm -rf ./obj_dir/

include ../Makefile
