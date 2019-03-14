# CS110 search Solution Makefile Hooks

SRC_DIR = src
BLD_DIR = build
RES_DIR = res

PROGS = letterboxedsolver
CLASSES = letterbox

CXX = /usr/bin/g++

CXX_WARNINGS = -Wall -pedantic
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES =

CXXFLAGS = $(CXX_WARNINGS) -Ofast -std=c++14 $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS =

PROGS_SRC = $(patsubst %,$(SRC_DIR)/%.cpp,$(PROGS))
PROGS_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BLD_DIR)/%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

CLASSES_SRC = $(patsubst %, $(SRC_DIR)/%.cpp, $(CLASSES))
CLASSES_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BLD_DIR)/%.o, $(CLASSES_SRC))
CLASSES_DEP = $(patsubst %.o,%.d,$(CLASSES_OBJ))

all:: make-build-folder $(PROGS)

$(PROGS): $(CLASSES_OBJ) $(PROGS_OBJ) copy-resources
	$(CXX) $(CLASSES_OBJ) $(PROGS_OBJ) -o $(addprefix $(BLD_DIR)/,$@) $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Copy all resource files into build folder
copy-resources:
	cp -a $(RES_DIR)/. $(BLD_DIR)

make-build-folder:
	@echo "Output is in folder '$(BLD_DIR)'."
	@mkdir -p $(BLD_DIR)

clean::
	rm -rf $(BLD_DIR)

.PHONY: all clean

-include $(PROGS_DEP)
