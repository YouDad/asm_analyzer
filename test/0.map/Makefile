targets := $(patsubst %.c, %, $(wildcard *.c))
test_objs := $(patsubst %.c, %.o, $(wildcard *.c))

.PHONY: default
default: $(test_objs) $(objs)
	for target in $(targets); do \
		cc -o $(notdir $(CURDIR)).$${target}.test $${target}.o $(objs); \
	done

.PHONY: install
install:
	mv *.test $(OUTDIR)/
