# axigate

AX.25 <> APRS-IS bidirectional gateway for networked TNCs.

### What it is

A bidirectional gateway that connects KISS TNCs (AX.25) to APRS-IS servers. It enables:

- **RF to Internet**: Forward packets from TNC to APRS-IS
- **Internet to RF**: Forward packets from APRS-IS to TNC
- **Callsign filtering**: Only forward packets for specific callsigns/paths
- **Duplicate suppression**: Prevent loops between RF and internet

### What it isn't

This project is **not**:

- A TNC — no modulation/demodulation, just a gateway
- A digipeater — doesn't rebroadcast RF packets on RF
- Cross-platform — Linux-only (uses Unix sockets, signal handling)

## Build and installation

### Prerequisites

- Linux
- GCC or Clang
- CMake

```bash
git clone https://github.com/xdab/axigate.git
cd axigate
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

## Usage

```bash
# TCP TNC connection with APRS-IS
axigate -h 192.168.0.9 -p 8144 -s aprs.server.com -P 14580 -c MYCALL -p 12345

# Unix socket TNC
axigate -x /run/tnc.sock -s aprs.server.com -P 14580 -c MYCALL -p 12345

# Configuration file
axigate -c axigate.conf

# Dry run (no packets transmitted)
axigate -n -c MYCALL -p 12345
```

## Command Line Arguments

| Short      | Long                | Description                         |
| ---------- | ------------------- | ----------------------------------- |
| `-c FILE`  | `--config=FILE`     | Configuration file                  |
| `-h ADDR`  | `--tnc-host=ADDR`   | TNC TCP address                     |
| `-p PORT`  | `--tnc-port=PORT`   | TNC TCP port                        |
| `-x SOCK`  | `--tnc-socket=SOCK` | TNC Unix socket path                |
| `-s ADDR`  | `--aprs-host=ADDR`  | APRS-IS server address              |
| `-P PORT`  | `--aprs-port=PORT`  | APRS-IS server port                 |
| `-C CALL`  | `--call=CALL`       | Gateway callsign                    |
| `-p PASS`  | `--passcode=PASS`   | APRS-IS passcode                    |
| `-f CALL`  | `--filter=CALL`     | Filter callsigns (comma-separated)  |
| `-v LEVEL` | `--log-level=LEVEL` | Log level: standard, verbose, debug |
| `-n`       | `--dry-run`         | Don't transmit packets              |

## Configuration File

Optionally, configuration can be read from a file using `-c FILE` or `--config=FILE`.

The file uses simple `key=value` syntax with `#` comments.

### Example

```ini
# axigate.conf
tnc-host=192.168.0.9
tnc-port=8144
aprs-host=aprs.server.com
aprs-port=14580
call=MYCALL
passcode=12345
filter=RELAY,WIDE
log-level=verbose
dry-run=false
```

## Dependencies

- **libtnc**: included as a [git submodule](libs/libtnc/)
  - AX.25 packet parsing/construction
  - KISS frame encoding/decoding
  - TCP/Unix socket utilities

## Installation

There is a helper Make target `make install` which handles everything from compilation to asking for SU rights and installing files to the relevant directories.

Systemd service `axigate.service` will be installed as well.
Please review the unit file and adjust for your needs.

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE)

---

**Development notes:** See [.clinerules](.clinerules) for AI-friendly technical documentation.
