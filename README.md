# apus-riot

RIOT board support for the **APUS board**, a development board built around the
STM32F072VBT6 (Arm Cortex-M0, 128 KB flash / 16 KB RAM).

[![CI](https://github.com/apus-org/apus-riot/actions/workflows/build.yml/badge.svg)](https://github.com/apus-org/apus-riot/actions/workflows/build.yml)

![APUS board](docs/img/apus-board.png)

## Overview

![Overview](docs/img/overview/overview.svg)

## Quick start

Clone RIOT and apus-riot next to each other:

```bash
git clone https://github.com/RIOT-OS/RIOT.git
git clone https://github.com/apus-org/apus-riot.git
```

Build and flash an example:

```bash
make -C RIOT/examples/basic/blinky BOARD=hsd-apus EXTERNAL_BOARD_DIRS=$PWD/apus-riot/boards flash
```

`make flash` uses OpenOCD through the on-board ST-Link.

## Requirements

RIOT `master` and the Arm GNU toolchain, or Docker with `BUILD_IN_DOCKER=1`. See the
[Getting Started guide](https://guide.riot-os.org/getting-started/installing/) for the
setup.

## Project layout

| Path | Contents |
|------|----------|
| [boards/](boards/) | board definition |
| [docs/](docs/) | documentation |

## License

Licensed under the GNU Lesser General Public License, version 2.1 ([LICENSE](LICENSE) or
<https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>).
