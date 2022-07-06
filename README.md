# HARM
HARM: Hardware-assisted Continuous Re-randomization for Microcontrollers

## General Idea

Code reuse attacks are particuarly noteworthy in microcontroller-based embedded systems since the memory address of firmware code is static. This work seeks to combat code reuse attacks, including ROP and more advanced JIT-ROP via continuous randomization.

## Citing our paper
```bibtex
@inproceedings{harm,
  title={HARM: Hardware-Assisted Continuous Re-randomization for Microcontrollers},
  author={Shi, Jiameng and Guan, Le and Li, Wenqiang and Zhang, Dayou and Chen, Ping and Zhang, Ning},
  booktitle={2022 IEEE 7th European Symposium on Security and Privacy (EuroS\&P)},
  pages={520--536},
  year={2022},
  organization={IEEE}
}
```

## Requirements

#### Requirements for taret MCU

`HARM` leverages TrustZone-M to isolate its secure runtime and firmware metadata from the target firmware code. Therefore, the target MCU processor must support ARMv8-M architecture and has security extension (i.e., TrustZone-M). Up to now, `HARM` is only tested under [NXP LPC55S69 development board](https://www.nxp.com/design/development-boards/lpcxpresso-boards/lpcxpresso55s69-development-board:LPC55S69-EVK). Please extend `HARM` to support more vendors.

#### Environment

* Unix-like OS, Debian/Ubuntu is recommended 
* Toolchain: [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm)
* Debugger such as SEGGER J-Link, which is used to flash the target board

## Usage
In this tutorial, we illustrate the usage of `HARM` under NXP LPC55S69 development board. First off, please clone `HARM` repository to your work directory.
```bash
git clone https://github.com/MCUSec/HARM.git
cd HARM
```

#### 1. Extract CMSE library

CMSE library is an ELF object file (named `secure_rt_cmse.o`) that contains the entry of the secure services (i.e., non-secure callable funtions that perform indirect calls, function returns, etc.). Calling of these functions will be instrumented to the target binary.   

```bash
cd scripts
./extract_cmse.sh lpc55s69 # secure_rt_cmse.o will be generated
```
#### 2. Instrument the binary

Please refer to the tutorial of `harm-rwtool`.

#### 3. Build HARM secure runtime
```bash
mkdir -p build && cd build
cmake -DTARGET_LPC55S69=On .. # LPC55s69 is the default option
make
```
After this step, you will get the binary of secure runtime named `harm_secure_rt.elf`. Then you can flash the secure runtime and the refined target binary to your NXP LPC55s69 development board.

## Adding Supports

You are welcomed to extend `HARM` to support more MCUs that satisfy the requirement. Please refer [here](supports/README.md) for details.

## Issues

If you encounter any problems with `HARM`, please open an issue. For other communications, please email jiameng @ uga.edu.

## HARM-Rust

We are building a safer `HARM` with Rust to avoid potential memory errors in the `HARM` secure runtime. `HARM-Rust` is coming soon. 