# AGENTS.md

axigate is an AX.25 ↔ APRS-IS bidirectional gateway for networked KISS TNCs. See [README.md](README.md) for what it is, installation, usage, and configuration; this document covers the developer/agent-facing view.

## Project layout

- `src/`, `include/` — application code; sources in `src/`, all headers in `include/`
  - `main.c` — entry point: options lifecycle, connections, optional UDP injection listeners, single-threaded event loop, signal handling
  - `options.c/h`, `options_args.c`, `options_file.c` — `options_t` lifecycle: init → CLI parse (argp) → config file parse → defaults
  - `connection.c/h` — TCP / Unix-socket TNC connection abstraction wrapping libcomm clients
  - `packet.c/h` — KISS decode/encode glue, TNC2 packet logging
  - `aprsis.c/h` — APRS-IS login string construction
  - `rxigate.c/h` — RF → APRS-IS: `prepare_for_rx_igate()` (validation, `qAR` annotation), `send_to_is()`
  - `txigate.c/h` — APRS-IS → RF: `prepare_for_tx_igate()` (path rewrite, third-party encapsulation), `send_to_tnc()`
- `libs/libtnc/` — submodule: AX.25 pack/unpack, KISS codec, TNC2 conversion, `key=value` conf parser, logging macros (`common.h`)
- `libs/libcomm/` — submodule: TCP/UDS clients, epoll-based `socket_poller`, line buffering
- `test/` — unit tests: `test_*.h` suites driven by `main_test.c`
- `systemd/` — `axigate.service`, installed by `make install`
- `sample.conf` — annotated config example

## Build & test

```bash
make build      # debug build into build/
make release    # release build
make test       # debug build + run unit tests (build/axigate_test)
make install    # release build + sudo make install (also installs systemd unit, daemon-reload)
make run        # debug build + run with sample.conf
make clean
make update     # git pull + submodule update
```

CMake targets: `axigate`, `axigate_test`. Debug adds `-pg -O0 -DDEBUG` (gprof instrumentation); Release adds `-O3 -DNDEBUG -flto`, links with `--gc-sections`, strips the binary post-build.

## Architecture

Single-threaded event loop in `main.c` with two process-wide globals shared by all modules: `options_t g_opts` and `ax25_addr_t g_igate_call`.

- Signal handlers (SIGINT/SIGTERM) only set `g_shutdown_requested`; the loop exits gracefully and frees connections via the `goto` chain.
- `socket_poller_wait()` (epoll, 1 s timeout) wakes on TNC or APRS-IS traffic.
- TNC bytes → `packet_decode()` (KISS → AX.25) → `prepare_for_rx_igate()` → `send_to_is()`.
- APRS-IS lines → `line_reader` → TNC2 parse → `prepare_for_tx_igate()` → `send_to_tnc()`.
- UDP datagrams (optional; KISS or TNC2 per listen port) → same rxigate path as TNC traffic, logged with marker `U`; gated only when `rf-to-is` is on.

Options precedence is mixed, by design of `options_file.c`: string keys from the config file only fill empty values (CLI wins); numeric/boolean keys present in the file override the CLI. Defaults fill the rest: TNC port 8144, APRS-IS port 14580, passcode -1, `rotate.aprs2.net`.

## Tech stack

- C11 with GNU extensions, CMake ≥ 3.10, Linux-only (epoll, argp, POSIX sockets)
- glibc `argp` for CLI parsing
- Vendored submodules `libtnc` and `libcomm`; links `tnc comm m`

## Conventions

- Function prefixes by module: `opts_*`, `connection_*`, `packet_*`, `aprsis_*`, `rxigate_*`, `txigate_*`
- Minimal comments — self-explanatory code preferred
- Returns: `0` = success, negative = error (libtnc style); some libtnc APIs return bool
- Allman braces, 4-space indent
- Buffers always passed as `buffer_t` (`data`/`capacity`/`size`); no heap allocation in the hot path
- `goto` for cleanup only in `main()`
- Logging via libtnc `common.h` macros to stderr: `LOG` (always), `LOGV` (verbose), `LOGD` (debug), `EXIT`/`EXITIF` (fatal). Messages start lowercase, no trailing period or newline (auto-added). Output prefixes: `i |`, `v |`, `d |`; traffic markers: `R` (RF→IS), `T` (IS login), `<` (from RF), `U` (UDP inject), `>` (gated to RF), `D` (dropped)

## Quality gates

- `make test` must pass — covers rxigate path annotation and txigate encapsulation
- No CI or linter configured; the test suite plus a clean compile is the gate

## Constraints

- Forwarding stays default-off; it must only run when explicitly enabled (`-r`/`-i` or config)
- Loop prevention is not optional: rxigate rejects `}`-encapsulated packets and `RFONLY`/`NOGATE` packets — keep those checks
- Do not modify anything under `libs/` in this repo; submodule changes belong upstream
- Linux-only by design: do not add portable-shim layers for Windows/macOS
- `prepare_for_rx_igate()` needs 2 free path slots (`qAR` + igate callsign); it returns -2 when the path is full

## Glossary

- **TNC** — terminal node controller, the AX.25 modem; reached via KISS over TCP or Unix socket
- **KISS** — minimal host-to-TNC framing protocol
- **APRS-IS** — central APRS internet servers; TNC2 text format, login is `user CALL pass N`
- **iGate** — internet gateway station; RX iGate = RF→IS, TX iGate = IS→RF
- **qAR** — APRS-IS path annotation meaning "received directly by an IGate"
- **Third-party packet** — `}`-encapsulated packet format used for TX igating
- **Passcode -1** — receive-only APRS-IS login (default); transmitting gated traffic needs a valid passcode

## Guiding principles

- Keep it small: single binary, two vendored libraries, no external dependencies
- Forward only what validates; drop and log everything else
