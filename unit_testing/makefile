project := ut
CPPFLAGS += -g -I../group_chat
CXXFLAGS += -Wfatal-errors
LINKFLAGS := -lgtest -lgtest_main -lpthread -lboost_regex -lboost_thread -lboost_program_options -lboost_system

extra_cpps := $(wildcard ../group_chat/*.cpp)
extra_objs := $(subst cpp,o,$(filter-out ../group_chat/main.cpp,$(extra_cpps)))

.PHONY: 

all: $(project)

include ../common.mk
