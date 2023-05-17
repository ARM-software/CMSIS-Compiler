# I/O Retarget example project

This project prints "Hello World" and a counter value via the UART output. It is configured for Arm Virtual Hardware, but other target hardware that provides a CMSIS Driver:USART can be easily added.

## Prerequisites

### Tools

- [CMSIS-Toolbox 1.7.0](https://github.com/Open-CMSIS-Pack/devtools/releases) or higher
- [Arm Compiler for Embedded](https://developer.arm.com/downloads/view/ACOMPE), or
- [GCC Compiler for Arm bare-metal](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [Arm Virtual Hardware](https://developer.arm.com/Tools%20and%20Software/Arm%20Virtual%20Hardware) for local execution (e.g. delivered as part of Keil MDK)

### Packs

- Required packs are listed in the file `retarget.csolution.yml`

## Project Structure

The project is generated using the [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/devtools/blob/main/tools/projmgr/docs/Manual/Overview.md) and is defined in [`csolution`](https://github.com/Open-CMSIS-Pack/devtools/blob/main/tools/projmgr/docs/Manual/YML-Input-Format.md) format:

- `retarget.csolution.yml` lists the required packs and defines the hardware target and build-types (along with the compiler).
- `retarget.cproject.yml` defines the source files and the software components.

## Build Project

Use the `cbuild` command to build the Debug configuration for VHT target defined in `.csolution.yml` solution file.

```bash
cbuild retarget.csolution.yml -v --update-rte -p --configuration .Debug+VHT
```

> Note: During the build process required packs may be downloaded (`-p` flag).

By default the project is compiled using Arm Compiler. One can change the compiler
selection in `.cdefault.yml` file to `GCC`.

## Execute Project

The project is configured for execution on Arm Virtual Hardware which removes the requirement for a physical hardware board.

```bash
VHT_MPS2_Cortex-M3 -f vht-cm3-config.txt out/retarget/VHT/Debug/retarget.axf
```

> Note: The application image file has extension `.elf` when using GCC Compiler.
