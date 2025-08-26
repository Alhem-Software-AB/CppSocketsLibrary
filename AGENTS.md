# AGENTS

## Build configuration
- Define `SOCKETS` as the `dist/` folder when consuming the Sockets library from Makefiles outside `src/`.
- Use `$(SOCKETS)/bin/` for helper binaries such as `Sockets-config`.
- Include headers from `$(SOCKETS)/include/`.
- Link against libraries in `$(SOCKETS)/lib/`.
- Set `PLATFORM` to the target platform (for example, `linux-x86-64`). Planned targets include `win32`, `win64`, `linux-x64-64`,
  `linux-x86-32`, and `raspberry-pi` (ARM).

  Supported values include:
  - `linux-x86-64`
  - `linux-x86-32`
  - `darwin-arm64`
  - `win32`
  - `win64`
  - `raspberry-pi`

  To derive a value, combine the lower-cased output of `uname -s` with `uname -m` (for example, `$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)`).
  The `Sockets-config` utility in `$(SOCKETS)/bin/` automatically prints the current platform and flags, and can be used to verify the selected `PLATFORM`.
- Define `INCLUDE`, `LIBS`, and `CFLAGS` before including `$(SOCKETS)/Makefile.version` and `$(SOCKETS)/Makefile.Defines.$(PLATFORM)` to reuse library-provided variables and recipes. These files append library-specific flags and copy `CFLAGS` to `CPPFLAGS` for C++ builds.

```make
SOCKETS =      ../dist
PLATFORM =     linux-x86-64

INCLUDE +=     -I$(SOCKETS)/include
LIBS +=        -L$(SOCKETS)/lib -lSockets
CFLAGS +=      $(INCLUDE)

include        $(SOCKETS)/Makefile.version
include        $(SOCKETS)/Makefile.Defines.$(PLATFORM)
```

## Makefile formatting
- Any Makefile other than the one in the src/ folder should follow the format used by src/Makefile.
- Values and dependencies must start at the second tab stop (column 17 with tab size 8), including the width of the prefix.
- If the prefix exceeds the width of two tab stops, end the line with a backslash and continue the value on the next line indented with two tabs.
- Any lines continued with backslash should be respected, and line break preserved.
- Surround assignment operators (`=`) with a single space on each side.
- Start recipe commands on a new line rather than using semicolons and indent them with exactly two tabs (the first being make's command prefix).

