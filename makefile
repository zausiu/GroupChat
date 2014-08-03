program := group_chat
utest := unit_testing

.PHONY: all $(program) $(utest)

all: $(program) $(utest) 

#$(program) $(utest):
$(program):
	$(MAKE) --directory=$@ $(TARGET)

clean:
#for d in $(program) $(utest);
	for d in $(program);                    \
	do                                      \
		$(MAKE) --directory=$$d clean;  \
	done
