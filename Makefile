
all:
		@$(MAKE)  -C src all dist

tests:		all
		@$(MAKE)  -C ./examples
		@$(MAKE)  -C ./demo
		@$(MAKE)  -C ./tests

clean:
		@$(MAKE)  -C ./src clean
		@$(MAKE)  -C ./examples clean
		@$(MAKE)  -C ./demo clean
		@$(MAKE)  -C ./tests clean
