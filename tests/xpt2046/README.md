## About

This is the test application for the XPT2046 touchscreen controller.

## Usage

The application works out of the box on the APUS board using the interrupt
mode by default:

```
make -C tests/xpt2046 flash term
```

To use the polling mode, the environment variable `XPT2046_POLLING_MODE` must
be set to 1. The polling period in milliseconds is defined by the environment
variable `XPT2046_POLLING_PERIOD`. It is 50 ms by default, for example:
```
XPT2046_POLLING_MODE=1 XPT2046_POLLING_PERIOD=100 make -C tests/xpt2046 flash term
```

## Expected output

The application initializes the XPT2046 and displays "Pressed!" when a touch
event is detected. While the screen is pressed, the position and the raw
conversion results are displayed every polling period. "Released!" is
displayed when the screen is released.

```
14:34:30 Pressed!
14:34:30 X: 121, Y: 157 (raw X: 2029, Y: 2078, Z: 1571)
14:34:30 X: 121, Y: 154 (raw X: 2030, Y: 2112, Z: 2539)
14:34:30 X: 120, Y: 154 (raw X: 2020, Y: 2111, Z: 2630)
14:34:32 Released!
14:35:49 Pressed!
14:35:49 X: 198, Y: 62 (raw X: 3172, Y: 3158, Z: 2618)
14:35:50 Released!
```
