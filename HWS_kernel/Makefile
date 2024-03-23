# Project Name (executable)
PROJECT = hws_kernel
# Project (executable) version
VERSION=1.0.0
# Compiler
CC = g++
# release or debug
MODE = debug
# Compiler options during compilation
COMPILE_OPTIONS = -Wall -g
# Linker options during linking
LINK_OPTIONS =
#Header include directories
#HEADERS =
#Libraries for linking
LIBS = 
# Dependency options
DEPENDENCY_OPTIONS=-MM
# Run Options       
COMMANDLINE_OPTIONS= 
# Directory to store *.o and *.d files
BUILDDIR=build
# Deploy path for remote building
REMOTE_USER=ubuntu
DEPLOYPATH=/usr/local/bin
IPADDR=192.168.1.223
# GDB
GDB_PORT=9091
#-- Do not edit below this line --

# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# $^ is the set of dependent files used to build something else
# $@ is the name of the target to be built.
# $< The name of the first prerequisite. If the target got its recipe from an implicit rule, this will be the first prerequisite added by the implicit rule

# Compile options
ifeq ($(MODE),release)
	COMPILE_OPTIONS += -O3 -flto
	LINK_OPTIONS += -flto
	BUILDDIR := $(BUILDDIR)/release
else
	COMPILE_OPTIONS += -g -O0
	BUILDDIR := $(BUILDDIR)/debug
endif

# Subdirs to search for additional source files
SOURCE_FILES := $(shell find . -name "*.cpp")

# Create an object file of every cpp file
OBJECTS = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(SOURCE_FILES))

# Dependencies
DEPENDENCIES = $(patsubst %.cpp, $(BUILDDIR)/%.d, $(SOURCE_FILES))

# Make $(PROJECT) the default target
.PHONY: all
all: $(BUILDDIR) $(DEPENDENCIES) $(BUILDDIR)/$(PROJECT)
# Create build directory
$(BUILDDIR):
	@echo "building directory $@"
	mkdir -p $@

# Create .d files
$(BUILDDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(DEPENDENCY_OPTIONS) $< -MT "$(BUILDDIR)/$*.o $(BUILDDIR)/$*.d" -MF $@
#	$(CC) $(DEPENDENCY_OPTIONS) $< -MT "$(BUILDDIR)/$*.o $(BUILDDIR)/$*.d" -MF $(BUILDDIR)/$*.d

# Compile every cpp file to an object
$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $(COMPILE_OPTIONS) -o $@ $< $(HEADERS)

# Link object files to executable
$(BUILDDIR)/$(PROJECT): $(OBJECTS)
	$(CC) $(LINK_OPTIONS) -o $(BUILDDIR)/$(PROJECT) $(OBJECTS) $(LIBS)

# Include dependencies (if there are any)
ifneq "$(strip $(DEPENDENCIES))" ""
  include $(DEPENDENCIES)
endif

remote:	all
	ssh $(REMOTE_USER)@$(IPADDR) "sudo pkill $(PROJECT) || true"
 	scp $(BUILDDIR)/debug/$(PROJECT) $(REMOTE_USER)@$(IPADDR):$(DEPLOYPATH)
#	ssh $(REMOTE_USER)@$(IPADDR) "sudo killall gdbserver || true"
	ssh -t $(REMOTE_USER)@$(IPADDR) "sudo gdbserver --multi :$(GDB_PORT) $(DEPLOYPATH)/$(PROJECT)"

# Build & Run Project
run: $(BUILDDIR)/$(PROJECT)
	$(BUILDDIR)/$(PROJECT) $(COMMANDLINE_OPTIONS)

rebuild: clean $(PROJECT)

# Clean & Debug
.PHONY: makefile-debug
makefile-debug:

.PHONY: clean
clean:
	rm -fR $(BUILDDIR)
	rm -f $(shell find . -name "*.o") $(shell find . -name "*.d")
#	rm -f *.o
#	rm -f $(PROJECT) $(BUILDDIR)/$(OBJECTS)

.PHONY: depclean
depclean:
#	rm -f $(BUILDDIR)/$(DEPENDENCIES)

clean-all: clean depclean