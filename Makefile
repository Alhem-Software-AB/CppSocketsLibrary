
all:
		@$(MAKE) -s -C src all dist

tests:		all
		@$(MAKE) -s -C ./src/tests
		@$(MAKE) -s -C ./demo/tests
		@$(MAKE) -s -C ./demo
		@$(MAKE) -s -C ./tests
