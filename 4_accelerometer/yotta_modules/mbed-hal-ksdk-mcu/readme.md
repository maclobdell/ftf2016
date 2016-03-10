# mbed HAL KSDK mcu

This is the HAL implementation for MCUs which are supported by KSDK. Part of this module is [Freescale KSDK](http://www.freescale.com/tools/software-and-tools/run-time-software/kinetis-software-and-tools/development-platforms-with-mbed/software-development-kit-for-kinetis-mcus:KINETIS-SDK) version 1.0.0.

## Freescale KSDK code

KSDK code is located under TARGET_KSDK_CODE. We had to do a couple of modifications to be able to use it in mbed OS.

- This module does not contain the entire KSDK. Only a few drivers and HAL are included.
- A header file like fsl_peripheral_features (peripheral here meaning any Freescale MCU peripheral, LPUART, DSPI) defines features for the MCU. In the original KSDK features files, it displayed an error ("No valid CPU defined"), so we defined MBED_NO_PERIPHERAL to remove the error.
