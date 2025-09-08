# PebbleOS bootloader

<a href="https://github.com/coredevices/pblboot/actions/workflows/build.yml?query=branch%3Amain">
  <img src="https://github.com/coredevices/pblboot/actions/workflows/build.yml/badge.svg?event=push">
</a>

This repository contains the PebbleOS bootloader.

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

The first step is to initialize the workspace folder where `pblboot` and all
Zephyr modules will be cloned. Run the following command:

```shell
west init -m git@github.com:coredevices/pblboot --mr main pblboot 
cd pblboot
west update
```

### Building and running

To build the bootloader, run the following command:

```shell
cd pblboot
west build -b $BOARD boot
```

where `$BOARD` is the target board, e.g. `pt2`.

Once you have built the bootloader, run the following command to flash it:

```shell
west flash
```
