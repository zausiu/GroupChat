##############################################################
# A makefile template serves a common purpose
# written by kamuszhou@tencent.com, kamuszhou@qq.com
# www.dogeye.net created on May 19, 2014
##############################################################

MV := mv -f
RM := rm -f
AR := ar rv

# pb files reside in the upper level directory
#pb_sources := $(shell ls ../*.proto)
#pb_ccs := $(addsuffix .pb.cc,$(basename $(notdir $(pb_sources))))
#pb_hs := $(addsuffix .pb.h,$(basename $(notdir $(pb_sources))))

sources      := $(wildcard *.cpp) $(wildcard *.cc)
stems        := $(basename $(sources))
objects      := $(addsuffix .o,$(stems))
dependencies := $(addsuffix .d,$(stems))

include_dirs := ../include
CPPFLAGS     += $(addprefix -I,$(include_dirs))

vpath %.h . $(include_dirs)

.PHONY: pre_build project library 

#pre_build: $(pb_ccs) $(pb.hs)

project: $(project)

library: $(library)

$(objects): $(pb_ccs) $(pb_hs)

$(project): $(objects) $(extra_objs)
	$(CXX) -o $@ $(objects) $(extra_objs) $(LINKFLAGS)

$(library): $(objects)
	$(AR) $@ $^

.PHONY: clean test
clean:
	$(RM) $(objects) $(program) $(library) $(dependencies) $(extra_clean) $(pb_ccs) $(pb_hs)

test:
	echo $(dependencies)

ifneq "$(MAKECMDGOALS)" "clean"
      -include $(dependencies)
endif

#$(call make-depend,source-file,object-file,depend-file) 
# -MM option causes g++ to omit "system" headers from the prerequisites list.
# This option implies -E which will stop after the preprocessing stage; do not
# run the compiler proper. 
# -MF option specifies the dependecy filename.
# -MT target change the target of the rule emitted by dependency generation.
define make-depend
  $(CXX) -MM -MF $3 -MT $2 $(CPPFLAGS) $(TARGET_ARCH) $1
endef

%.o : %.cpp
	$(call make-depend, $<,$@,$(subst .cpp,.d,$<))
	$(COMPILE.C) $(OUTPUT_OPTION) $<

#%.pb.cc %.pb.h: ../%.proto
#protoc $< --cpp_out=. --proto_path=..

#%.pb.o : %.pb.cc
#$(COMPILE.C) $(OUTPUT_OPTION) $<
