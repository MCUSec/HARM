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

#### Requirements for target device

HARM leverages TrustZone-M to isolate its secure runtime and firmware metadata from the target firmware binary code. It supports ARM Cortex-M seris MCUs with security extension, such as Cortex-M33 MCU. HARM is tested under NXP LPC55S69 development board, we hope HARM can be tested on more devices.

#### Requirements for target binary

The target binary

* must be compiled as ARM Thumb-2 code for ARM Cortex-M33
* must not be stripped, and reserve relocations (through `-Wl,-q` link flag)

## Tutorial



