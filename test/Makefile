test_objs := $(patsubst %.c, %.o, $(shell find * -name "*.c"))

.PHONY: default
default: $(test_objs) $(objs)
	for object in $(shell find * -name "*.c" | sed 's/\.c//;'); do \
		filename=`echo $${object} | sed 's/\//./g;'`; \
		cc -o $${filename}.test $${object}.o $(objs); \
	done;

.PHONY: install
install:
	mv *.test $(OUTDIR)/
