PSNOOB_DIR=/home/bj/d_drive/Programming/psx/psn00bsdk
include $(PSNOOB_DIR)/examples/examples-setup.mk

# Project target name
TARGET		= hello.elf

# Searches for C, C++ and S (assembler) files in local directory
CFILES		= $(notdir $(wildcard *.c))
CPPFILES 	= $(notdir $(wildcard *.cpp))
AFILES		= $(notdir $(wildcard *.s))

# Determine object files
OFILES		= $(addprefix build/,$(CFILES:.c=.o)) \
			$(addprefix build/,$(CPPFILES:.cpp=.o)) \
			$(addprefix build/,$(AFILES:.s=.o))

# Project specific include and library directories
# (use -I for include dirs, -L for library dirs)
INCLUDE	 	+=
LIBDIRS		+= -L$(PSNOOB_DIR)/libpsn00b

# Libraries to link
LIBS		= -lc

# C compiler flags
CFLAGS		= -g -g3 -ggdb -O0 -fno-builtin -fdata-sections -ffunction-sections

# C++ compiler flags
CPPFLAGS	= $(CFLAGS) -fno-exceptions

# Assembler flags
AFLAGS		= -g -msoft-float

# Linker flags
LDFLAGS		= -g -Ttext=0x80010000 -gc-sections \
			-T $(GCC_BASE)/mipsel-unknown-elf/lib/ldscripts/elf32elmip.x

all: $(OFILES)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OFILES) $(LIBS) -o $(TARGET)
	elf2x -q $(TARGET)
	
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(AFLAGS) $(INCLUDE) -c $< -o $@
	
build/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(AFLAGS) $(INCLUDE) -c $< -o $@
	
clean:
	rm -rf build $(TARGET) $(TARGET:.elf=.exe)
