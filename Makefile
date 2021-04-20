# GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug_win32
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug_win32)
  freetype_config = debug_win32
  deng_config = debug_win32
  dam_config = debug_win32
endif
ifeq ($(config),debug_linux)
  freetype_config = debug_linux
  deng_config = debug_linux
  dam_config = debug_linux
endif
ifeq ($(config),release_win32)
  freetype_config = release_win32
  deng_config = release_win32
  dam_config = release_win32
endif
ifeq ($(config),release_linux)
  freetype_config = release_linux
  deng_config = release_linux
  dam_config = release_linux
endif

PROJECTS := freetype deng dam

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

freetype:
ifneq (,$(freetype_config))
	@echo "==== Building freetype ($(freetype_config)) ===="
	@${MAKE} --no-print-directory -C . -f freetype.make config=$(freetype_config)
endif

deng: freetype
ifneq (,$(deng_config))
	@echo "==== Building deng ($(deng_config)) ===="
	@${MAKE} --no-print-directory -C . -f deng.make config=$(deng_config)
endif

dam:
ifneq (,$(dam_config))
	@echo "==== Building dam ($(dam_config)) ===="
	@${MAKE} --no-print-directory -C . -f dam.make config=$(dam_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f freetype.make clean
	@${MAKE} --no-print-directory -C . -f deng.make clean
	@${MAKE} --no-print-directory -C . -f dam.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug_win32"
	@echo "  debug_linux"
	@echo "  release_win32"
	@echo "  release_linux"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   freetype"
	@echo "   deng"
	@echo "   dam"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"