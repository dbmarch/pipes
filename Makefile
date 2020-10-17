SHELL = /bin/bash
SUBDIRS   := lib/ example/

all: 
	@for dir in $(SUBDIRS); do \
		echo "processing folder $$dir"; \
		make -C $$dir ; \
		done;
		@echo "Done"

clean:
	@for dir in $(SUBDIRS); do \
		echo "processing folder $$dir"; \
		make -C $$dir clean; \
		done;
		@echo "Done"

.PHONY: all clean 