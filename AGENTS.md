# AGENTS

## Build configuration
- Define `SOCKETS` as the `dist/` folder when consuming the Sockets library from Makefiles outside `src/`.
- Use `$(SOCKETS)/bin/` for helper binaries such as `Sockets-config`.
- Include headers from `$(SOCKETS)/include/`.
- Link against libraries in `$(SOCKETS)/lib/`.
