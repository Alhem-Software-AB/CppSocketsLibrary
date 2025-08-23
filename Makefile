
all:
		@$(MAKE) -C src all dist

tests:		all
		@$(MAKE) -C ./examples
		@$(MAKE) -C ./examples/simple-http-server-demo
		@$(MAKE) -C ./tests

clean:
		@$(MAKE) -C ./src clean
		@$(MAKE) -C ./examples clean
		@$(MAKE) -C ./examples/simple-http-server-demo clean
		@$(MAKE) -C ./tests clean
