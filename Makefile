# CS110 search Solution Makefile Hooks

SRC_DIR = src
BLD_DIR = build
RES_DIR = res

PROGS = letterboxedsolver

CXX = /usr/bin/g++

CXX_WARNINGS = -Wall -pedantic
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES =

CXXFLAGS = $(CXX_WARNINGS) -O2 -std=c++14 $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS =

PROGS_SRC = $(addprefix $(SRC_DIR)/, $(patsubst %,%.cpp,$(PROGS)))
PROGS_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BLD_DIR)/%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all:: $(PROGS)

$(PROGS): $(PROGS_OBJ) copy-resources
	mkdir -p $(BLD_DIR)
	$(CXX) $(PROGS_OBJ) -o $(addprefix $(BLD_DIR)/,$@) $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BLD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Copy all resource files into build folder
copy-resources:
	cp -a $(RES_DIR)/. $(BLD_DIR)

clean::
	rm -rf $(BLD_DIR)

.PHONY: all clean

-include $(PROGS_DEP)