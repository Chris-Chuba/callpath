# Compiler
CXX = gcc

# Compiler flags
# -rdynamic and -lbacktrace provide symbols for stack backtrace
CXXFLAGS = -Wall -rdynamic -lbacktrace  -g -std=c18

# Source file
SRC = testcallpath.c
HDR = callpath.h

# Object files
OBJDIR = o
OBJS = $(addprefix $(OBJDIR)/,$(SRC:.c=.o))

# Executable name
EXEC = testcallpath

# Default target
all: $(EXEC)
	@echo "Executable '$(EXEC)' rebuilt."

# Rule to compile the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files
$(OBJDIR)/%.o: %.c $(HDR) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create the o subdirectory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule
clean:
	rm -f $(OBJDIR)/* $(EXEC)
