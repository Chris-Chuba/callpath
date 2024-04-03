# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -rdynamic -lbacktrace  -g

# Source file
SRC = testcallpath.cxx
HDR = callpath.hxx

# Object files
OBJDIR = o
OBJS = $(addprefix $(OBJDIR)/,$(SRC:.cxx=.o))

# Executable name
EXEC = testcallpath

# Default target
all: $(EXEC)
	@echo "Executable '$(EXEC)' rebuilt."

# Rule to compile the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files
$(OBJDIR)/%.o: %.cxx $(HDR) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create the o subdirectory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule
clean:
	rm -f $(OBJDIR)/* $(EXEC)
