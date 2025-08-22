# AGENTS

## Build configuration
- Define `SOCKETS` as the `dist/` folder when consuming the Sockets library from Makefiles outside `src/`.
- Use `$(SOCKETS)/bin/` for helper binaries such as `Sockets-config`.
- Include headers from `$(SOCKETS)/include/`.
- Link against libraries in `$(SOCKETS)/lib/`.

## Makefile formatting
- Any Makefile other the one in the src/ folder should follow the format used by src/Makefile.
- Values and dependencies should be indented a total of two tabs, including prefix.
- Any lines continued with backslash should be respected, and line break preserved.
