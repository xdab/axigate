# axigate

AX.25 ↔ APRS-IS bidirectional gateway for networked TNCs. Forwards packets bidirectionally between KISS TNCs and APRS-IS servers.

### What it is

A bidirectional gateway that connects KISS TNCs (AX.25) to APRS-IS servers:

- **RF to Internet (RX-iGate)**: Decode packets from TNC and forward to APRS-IS with path annotation
- **Internet to RF (TX-iGate)**: Receive packets from APRS-IS and transmit to RF via TNC (encapsulated format)
- **Path annotation**: Adds `qAR` and gateway callsign to packet path for RF→IS packets
- **Directional control**: Enable or disable each direction independently

### Quick Start

```bash
# Enable RF to APRS-IS forwarding only (RX-iGate mode)
axigate -h 192.168.0.9 -p 8144 -s aprs.server.com -P 14580 -c MYCALL -r

# Enable both directions (bidirectional gateway)
axigate -h 192.168.0.9 -p 8144 -s aprs.server.com -P 14580 -c MYCALL -r -i

# Unix socket TNC
axigate -x /run/tnc.sock -s aprs.server.com -P 14580 -c MYCALL -r

# Configuration file
axigate -c axigate.conf
```

**Important**: By default, no forwarding is enabled. You must use `-r` (RF→IS) and/or `-i` (IS→RF) flags to enable packet forwarding.

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

## Command Line Arguments

| Short       | Long                 | Description                              |
| ----------- | -------------------- | ---------------------------------------- |
| `-c FILE`   | `--config=FILE`      | Configuration file                       |
| `-h ADDR`   | `--host=ADDR`        | TNC TCP address                          |
| `-p PORT`   | `--port=PORT`        | TNC TCP port (default: 8144)             |
| `-x SOCK`   | `--socket=SOCK`      | TNC Unix socket path                     |
| `-s SSID`   | `--ssid=SSID`        | Gateway SSID (default: 0)                |
| `-S ADDR`   | `--is-host=ADDR`     | APRS-IS server address (default: rotate.aprs2.net) |
| `-P PORT`   | `--is-port=PORT`     | APRS-IS server port (default: 14580)     |
| `-f FILTER` | `--is-filter=FILTER` | APRS-IS filter string                    |
| `-C CALL`   | `--call=CALL`        | Gateway callsign                         |
| `-r`        | `--rf-to-is`         | Enable RF to APRS-IS forwarding          |
| `-i`        | `--is-to-rf`         | Enable APRS-IS to RF forwarding          |
| `-v`        |                      | Verbose logging                          |
| `-V`        |                      | Debug logging                            |

## Configuration File

Optionally, configuration can be read from a file using `-c FILE` or `--config=FILE`.

The file uses simple `key=value` syntax with `#` comments.

### Example

```ini
# axigate.conf
# TNC connection (TCP or Unix socket)
host=192.168.0.9
port=8144
# socket=/tmp/tnc.sock

# Gateway identity (for APRS-IS path annotation)
call=MYCALL
ssid=0

# APRS-IS connection
is-host=aprs.server.com
is-port=14580
is-filter=m/20
is-passcode=12345

# Enable forwarding directions
rf-to-is=true
is-to-rf=false

# Logging: standard, verbose, debug
log-level=verbose
```

## How It Works

### RF to Internet (RX-iGate)

1. Receive KISS frames from TNC
2. Decode AX.25 packet
3. Validate packet (check for RFONLY/NOGATE, ignore encapsulated packets)
4. Add `qAR` and gateway callsign to path
5. Convert to TNC2 format and send to APRS-IS

### Internet to RF (TX-iGate)

1. Receive packet from APRS-IS (TNC2 format)
2. Parse AX.25 packet
3. Replace path with TCPIP marker and gateway callsign
4. Encapsulate original packet in `}` format
5. Send to TNC as KISS frame

## Dependencies

- **libtnc**: included as a [git submodule](libs/libtnc/)
  - AX.25 packet parsing/construction
  - KISS frame encoding/decoding
  - TCP/Unix socket utilities
- **libcomm**: included as a [git submodule](libs/libcomm/)
  - TCP/Unix socket clients
  - Socket polling

## Installation

There is a helper Make target `make install` which handles everything from compilation to asking for SU rights and installing files to the relevant directories.

Systemd service `axigate.service` will be installed as well.
Please review the unit file and adjust for your needs.

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE)

---

**Development notes:** See [.clinerules](.clinerules) for AI-friendly technical documentation.
