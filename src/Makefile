main_objs := $(patsubst %.c, %.o, $(wildcard *.c))
.PHONY: hhee_analysis
hhee_analysis: $(main_objs) $(objs)
	cc -o hhee_analysis $(main_objs) $(objs)

.PHONY: install
install:
	mv hhee_analysis $(OUTDIR)/

.PHONY: clean
clean:
	-rm *.o
