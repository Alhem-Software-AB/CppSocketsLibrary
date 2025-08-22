
all:
		@$(MAKE)  -C src all dist

tests:		all
		@$(MAKE)  -C ./src/tests
		@$(MAKE)  -C ./demo/tests
		@$(MAKE)  -C ./demo
		@$(MAKE)  -C ./tests

clean:
		@$(MAKE)  -C ./src clean
		@$(MAKE)  -C ./src/tests clean
		@$(MAKE)  -C ./demo clean
		@$(MAKE)  -C ./demo/tests clean
		@$(MAKE)  -C ./tests clean
