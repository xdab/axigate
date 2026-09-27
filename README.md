# axigate

AX.25 ↔ APRS-IS bidirectional gateway for networked TNCs. Connects to a KISS TNC over TCP or Unix socket and forwards packets to and from APRS-IS servers.

## Features

- **RF to Internet (RX iGate)**: decode packets from the TNC and forward them to APRS-IS with `qAR` path annotation
- **Internet to RF (TX iGate)**: receive packets from APRS-IS and transmit them to RF via the TNC as encapsulated third-party packets
- **Independent directions**: enable either or both forwarding directions
- **TCP or Unix socket TNC connection**

## Installation

Requirements: Linux, GCC or Clang, CMake.

```bash
git clone --recurse-submodules https://github.com/xdab/axigate.git
cd axigate
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

Alternatively, `make release` builds and `make install` builds and installs.

`make install` also installs the `axigate.service` systemd unit to `/etc/systemd/system` and reloads the daemon. Review the unit file and adjust it to your setup.

The build depends on two bundled git submodules: [libtnc](libs/libtnc/) (AX.25 parsing, KISS framing) and [libcomm](libs/libcomm/) (socket clients, polling).

## Usage

By default no forwarding is enabled — pass `-r` and/or `-i` to turn directions on.

```bash
# RX iGate only: RF packets forwarded to APRS-IS
axigate --host 192.168.0.9 --port 8144 --call MYCALL --rf-to-is

# Bidirectional gateway
axigate --host 192.168.0.9 --port 8144 --call MYCALL --rf-to-is --is-to-rf

# TNC behind a Unix socket
axigate --socket /run/tnc.sock --call MYCALL --rf-to-is

# All options from a configuration file
axigate --config axigate.conf
```

## Command-line options

| Option | Description |
| --- | --- |
| `-c, --config=FILE` | Configuration file |
| `-h, --host=ADDR` | TNC host address |
| `-p, --port=PORT` | TNC TCP port (default: 8144) |
| `-x, --socket=PATH` | TNC Unix socket path |
| `--is-host=HOST` | APRS-IS server (default: rotate.aprs2.net) |
| `--is-port=PORT` | APRS-IS port (default: 14580) |
| `--is-filter=FILTER` | APRS-IS filter string |
| `--is-passcode=N` | APRS-IS passcode (default: -1) |
| `--is-keepalive=N` | Minimum seconds between APRS-IS keepalive logins (default: 300, 0 = off) |
| `--udp-kiss-listen=PORT` | UDP port listening for KISS packets to gate to APRS-IS (default: off) |
| `--udp-tnc2-listen=PORT` | UDP port listening for TNC2 packets to gate to APRS-IS (default: off) |
| `-C, --call=CALL` | Gateway callsign |
| `-s, --ssid=N` | Gateway SSID (default: 0) |
| `-r, --rf-to-is` | Enable RF to APRS-IS forwarding |
| `-i, --is-to-rf` | Enable APRS-IS to RF forwarding |
| `-v, --verbose` | Verbose logging |
| `-V, --debug` | Debug logging |

## Configuration file

With `-c FILE` / `--config=FILE`, options can be read from a file using `key=value` syntax and `#` comments. Keys mirror the long option names; see [sample.conf](sample.conf) for an annotated example.

```ini
# TNC connection (TCP or Unix socket)
host=192.168.0.9
port=8144
# socket=/run/tnc.sock

# Gateway identity (used for APRS-IS path annotation)
call=MYCALL
ssid=0

# APRS-IS connection
is-host=rotate.aprs2.net
is-port=14580
is-filter=m/20
is-passcode=-1
# is-keepalive=300  # minimum seconds between keepalive logins, 0 = off

# UDP injection inputs (0 = off); requires rf-to-is=true
# udp-kiss-listen=0
# udp-tnc2-listen=0

# Forwarding directions
rf-to-is=true
is-to-rf=false

# Log level: verbose or debug (omit for standard)
verbose=verbose
```

## How it works

RF to APRS-IS:

1. Receive KISS frames from the TNC and decode them into AX.25 packets
2. Skip packets with empty payloads, packets containing `RFONLY`/`NOGATE`, and already-encapsulated packets (loop prevention)
3. Append `qAR` and the gateway callsign to the path
4. Convert to TNC2 format and send to APRS-IS

APRS-IS to RF:

1. Receive packets from APRS-IS in TNC2 format
2. Replace the path with the `TCPIP` marker and the gateway callsign
3. Encapsulate the original packet as a `}` third-party packet
4. Encode as KISS and send to the TNC

UDP injection:

Local tools (e.g. a cron job beaconing the igate status) can push packets to
APRS-IS without a TNC. With `--udp-tnc2-listen=PORT` or `--udp-kiss-listen=PORT`,
axigate listens for datagrams in TNC2 or KISS format and gates them like RF
traffic (same validation, `qAR` annotation, loop prevention; requires
`rf-to-is`). Each datagram should hold one KISS frame or one newline-terminated
TNC2 line, e.g.:

```bash
echo 'MYCALL>APRS:>igate up' | nc -u -w1 127.0.0.1 28145
```

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).
