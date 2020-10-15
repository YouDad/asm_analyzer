export
SRCDIR := $(CURDIR)/libs
OUTDIR := $(CURDIR)/out
objs := $(patsubst %.c, %.o, $(wildcard $(SRCDIR)/*.c))
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
	make -C test/
	make install -C test/
	./test.sh
	make clean

.PHONY: clean
clean:
	-find -name "*.o" | xargs rm
