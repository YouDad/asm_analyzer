targets := $(wildcard */)

.PHONY: default
default:
	for target in $(targets); do \
		make -C $${target}/; \
	done

.PHONY: install
install:
	for target in $(targets); do \
		make install -C $${target}/; \
	done
