#!/bin/bash

OBJ_OUTPUT=nsc.o
NSCLIB_OUTPUT=secure_rt_nsc.o

CC=arm-none-eabi-gcc
CFLAGS="-ffunction-sections -fdata-sections -fno-exceptions -ffreestanding -fno-builtin -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -mcmse"
LDFLAGS="${CFLAGS} -mthumb -nostdlib -Wl,--cmse-implib -T $PWD/../supports/$1/$1.ld"

$CC ${CFLAGS} -c $PWD/../nsc.c
$CC ${LDFLAGS} ${OBJ_OUTPUT} -Wl,--out-implib=$PWD/../supports/$1/secure_rt_nsc.o 2>/dev/null
rm -f ${OBJ_OUTPUT}