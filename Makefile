all: hhee_analysis clean
	echo compile complete!

include src/Makefile

hhee_analysis: $(obj-y)
	cc -o hhee_analysis $(obj-y)

.PHONY: clean
clean:
	rm $(obj-y)
