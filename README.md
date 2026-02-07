# axigate

AX.25 RX-iGate for networked TNCs. Forwards packets from KISS TNCs to APRS-IS.

### What it is

An RX-iGate that connects KISS TNCs (AX.25) to APRS-IS servers:

- **RF to Internet**: Decode packets from TNC and forward to APRS-IS
- **Path annotation**: Adds `qAR` and gateway callsign to packet path
- **Dry run mode**: Test configuration without transmitting packets

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

| Short       | Long                 | Description                          |
| ----------- | -------------------- | ------------------------------------ |
| `-c FILE`   | `--config=FILE`      | Configuration file                   |
| `-h ADDR`   | `--tnc-host=ADDR`    | TNC TCP address                      |
| `-p PORT`   | `--tnc-port=PORT`    | TNC TCP port (default: 8144)         |
| `-x SOCK`   | `--tnc-socket=SOCK`  | TNC Unix socket path                 |
| `-s ADDR`   | `--ssid=SSID`        | Gateway SSID (default: 0)            |
| `-S ADDR`   | `--aprs-host=ADDR`   | APRS-IS server address               |
| `-P PORT`   | `--aprs-port=PORT`   | APRS-IS server port (default: 14580) |
| `-C CALL`   | `--call=CALL`        | Gateway callsign                     |
| `-f FILTER` | `--is-filter=FILTER` | APRS-IS filter string                |
| `-p PASS`   | `--is-passcode=PASS` | APRS-IS passcode                     |
| `-v`        |                      | Verbose logging                      |
| `-V`        |                      | Debug logging                        |
| `-n`        | `--dry-run`          | Don't transmit packets               |

## Configuration File

Optionally, configuration can be read from a file using `-c FILE` or `--config=FILE`.

The file uses simple `key=value` syntax with `#` comments.

### Example

```ini
# axigate.conf
tnc-host=192.168.0.9
tnc-port=8144
is-host=aprs.server.com
is-port=14580
is-filter=m/20
is-passcode=12345
call=MYCALL
ssid=0
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
