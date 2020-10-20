export
OUTDIR := $(CURDIR)/out
objs := $(patsubst %.c, %.o, $(wildcard $(CURDIR)/src/*/*.c))
CFLAGS := -g
CFLAGS += -I$(CURDIR)/include/

.PHONY: default
default: src clean

.PHONY: src
src:
	make -C src/
	make install -C src/

.PHONY: test
test: src
	make clean
	make -C test/
	rm -rf $(OUTDIR)
	mkdir -p $(OUTDIR)
	make install -C test/
	make clean

.PHONY: clean
clean:
	-find -name "*.o" | xargs rm -f
