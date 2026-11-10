## About

This is the test application for the MCP79410 real-time clock.

## Usage

The application works out of the box on the APUS board:

```
make -C tests/mcp79410 flash term
```

It provides shell commands to read and set the time, to configure the alarm
and the square-wave output, and to stop and start the oscillator. The `test`
command runs a self-test. It sets the time, so set the correct time again
afterwards.

## Expected output

```
> test
[test] setting 2026-11-10T23:59:58 (wday 2)
[test] read back 2026-11-11T00:00:00 (wday 3)
[test] time and day rollover OK
[test] setting 2028-02-29T23:59:58 (wday 2)
[test] read back 2028-03-01T00:00:01 (wday 3)
[test] time and day rollover OK
[test] alarm OK
[test] SRAM OK
[test] SUCCESS
```
