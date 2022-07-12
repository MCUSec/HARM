# HARM rwtool

Binary rewriting tool for HARM

## Environment Setup

`rwtool` is implemented in python3. Make sure python3 and python3-venv is installed. `rwtool` depends on [capstone]() and [keystone]() to disassemble and reassamble the target binary. 

1. Run `setup.sh` to install all dependencies
    ```bash
    ./setup.sh
    ```

2. Activate the virtual environment
    ```bash
    . harm/bin/activate
    ```


## Usage

1. Please make sure the binary:
    - Must be compiled as ARM thumb-2 code for Cortex-M seris MCUs
    - Not stripped and reserve the relocation table.

2. Use `rwtool` to instrument the target binary:
    
    ```bash
    cd $HARM_HOME  # $HARM_HOME has been defined in the virtual environment
    rwtool -i path/to/target_binary.elf -e <entry point> -o path/to/refined_binary.bin
    ```
    - `path/to/target_binary.elf`: path of the target binary to be instrumented
    - `<entry_point>`: the address of the target binary to be flashed
    - `path/to/refined_binary.bin`: output path of the refined binary. 
