.phony: clean

#----------------------------#
# Directories
#----------------------------#

OBJDIR := build
RELDIR := release
INCDIR := include
SRCDIR := source

#----------------------------#
# Cross-compilation, compile flags
#----------------------------#

ifeq ($(strip $(ARCHCROSS)),)
	ARCHCROSS :=
endif
ifeq ($(strip $(ARCHFLAGS)),)
	ARCHFLAGS :=
endif

CCFLAGS := $(ARCHFLAGS) -O2 -Wall -Wextra $(foreach dir, $(INCDIR), -I$(dir))

#----------------------------#
# Tools
#----------------------------#

CC := $(ARCHCROSS)gcc
AR := $(ARCHCROSS)ar

#----------------------------#
# Platform detection
#----------------------------#

ifeq ($(strip $(PLATFORM)),)
	DUMPMACHINE := $(shell $(CC) -dumpmachine)
	ifneq ($(filter x86%,$(DUMPMACHINE)),)
		PLATFORM := x86
	endif
	ifneq ($(filter arm%,$(DUMPMACHINE)),)
		PLATFORM := arm
	endif
	# etc.
endif

#----------------------------#
# Source files
#----------------------------#

SRCFILES := $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))

#----------------------------#
# Add platform-specific files
#----------------------------#

ifeq ($(PLATFORM), x86)
	SRCFILES += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/x86/*.c))
endif

#----------------------------#
# Output files
#----------------------------#

OBJFILES := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(SRCFILES)))
LIBFILE  := $(RELDIR)/libfourier.a

DFILES := $(OBJFILES:.o=.d)

#----------------------------#
# make all
#----------------------------#

all : $(LIBFILE)

$(OBJDIR) $(RELDIR) :; mkdir -p $@

$(LIBFILE) : $(OBJFILES) | $(RELDIR)
	@echo Building library file $@...
	@$(AR) rcs $@ $^

#----------------------------#
# x86-specific rules
#----------------------------#

ifeq ($(PLATFORM), x86)
$(OBJDIR)/%_AVX_FMA.c.o : %_AVX_FMA.c | $(OBJDIR)
	@echo [x86] $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c -MD -MP -MF $(OBJDIR)/$<.d -o $@ $< -mavx -mfma

$(OBJDIR)/%_AVX.c.o : %_AVX.c | $(OBJDIR)
	@echo [x86] $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c -MD -MP -MF $(OBJDIR)/$<.d -o $@ $< -mavx

$(OBJDIR)/%_SSE_FMA.c.o : %_SSE_FMA.c | $(OBJDIR)
	@echo [x86] $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c -MD -MP -MF $(OBJDIR)/$<.d -o $@ $< -msse -mfma

$(OBJDIR)/%_SSE.c.o : %_SSE.c | $(OBJDIR)
	@echo [x86] $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c -MD -MP -MF $(OBJDIR)/$<.d -o $@ $< -msse
endif

#----------------------------#
# Generic rules
#----------------------------#

$(OBJDIR)/%.c.o : %.c | $(OBJDIR)
	@echo $(notdir $<)
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -c -MD -MP -MF $(OBJDIR)/$<.d -o $@ $<

#----------------------------#
# Dependencies
#----------------------------#

-include $(DFILES)

#----------------------------#
# make clean
#----------------------------#

clean :; rm -rf $(OBJDIR) $(RELDIR)

#----------------------------#
