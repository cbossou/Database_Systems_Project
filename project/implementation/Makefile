##### ROCKSDB STUFF #####

INSTALL_PATH ?= /usr/local

# if user didn't config LIBNAME, set the default
ifeq ($(LIBNAME),)
	# we should only run rocksdb in production with DEBUG_LEVEL 0
	ifeq ($(DEBUG_LEVEL),0)
		LIBNAME=librocksdb
	else
		LIBNAME=librocksdb_debug
	endif
endif

LIBRARY = ${LIBNAME}.a

INSTALL_FILES = $(INSTALL_PATH)/include/rocksdb \
								$(INSTALL_PATH)/lib/$(LIBRARY)


CONFIG_FILE = $(INSTALL_PATH)/include/rocksdb/make_config.mk

#ifeq ("$(wildcard $(CONFIG_FILE))","")
ifneq ("$(CONFIG_FILE)","")
	include $(CONFIG_FILE)
else
  $(error Rocksdb make configuration not yet generated. Please install rocksdb)
endif

ifndef DISABLE_JEMALLOC
	ifdef JEMALLOC
		PLATFORM_CXXFLAGS += -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE
	endif
	EXEC_LDFLAGS := $(JEMALLOC_LIB) $(EXEC_LDFLAGS)
	PLATFORM_CXXFLAGS += $(JEMALLOC_INCLUDE)
endif

ifneq ($(USE_RTTI), 1)
	CXXFLAGS += -fno-rtti
endif

ROCKSDBCOMPFLASGS = $(CXXFLAGS)

ROCKSDBLINKFLAGS = $(INSTALL_PATH)/lib/librocksdb.a -I$(INSTALL_PATH)/include \
									 -O2 -std=c++11 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) \
									 $(EXEC_LDFLAGS)

##### END ROCKSDB STUFF #####

##### GENERAL MAKEFILE #####

CPPSRCEXT = cc
CPPHEADEXT = h
MAIN = Main
MODULES = ExtraUtil BenchMark
HEADERS = $(MODULES:%=%.$(CPPHEADEXT))
SRC = $(MAIN:%=%.$(CPPSRCEXT)) $(MODULES:%=%.$(CPPSRCEXT))
OBJ = $(SRC:%.$(CPPSRCEXT)=%.o)
EXE = RocksBench
CPP = $(CXX)
CPPCOMP = -c
CPPLINK = -o
CPPCOMPFLAGS = --std=c++11 -Wall -Werror -pedantic
CPPCOMPFLAGS += $(ROCKSDBCOMPFLASGS)
CPPLINKFLAGS = -lboost_filesystem -lboost_system
CPPLINKFLAGS += $(ROCKSDBLINKFLAGS)
RESULTSDIR = results

all: $(EXE)

$(EXE): $(OBJ)
	$(CPP) $(CPPLINK) $@ $^ $(CPPLINKFLAGS)

%.o: %.$(CPPSRCEXT)
	$(CPP) $(CPPCOMPFLAGS) $(CPPCOMP) $^

clean:
	rm -f $(OBJ)

spotless: clean
	rm -rf $(EXE) $(RESULTSDIR)

.PHONY: all clean spotless

##### END GENERAL MAKEFILE #####

