# I/O Retarget example project {#rt_example}

This project prints "Hello World" and a counter value via the UART output. It is configured for Arm Virtual Hardware, but other target hardware that provides a CMSIS Driver:USART can be easily added.

## Prerequisites

### Tools

- [CMSIS-Toolbox 2.10.0](https://github.com/Open-CMSIS-Pack/devtools/releases) or higher
- [Arm Compiler for Embedded](https://developer.arm.com/downloads/view/ACOMPE), or
- [GCC Compiler for Arm bare-metal](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) or
- [Clang Compiler](https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm) or
- [IAR Build Tools for Arm](https://www.iar.com/embedded-development-tools)
- [Arm Virtual Hardware](https://developer.arm.com/Tools%20and%20Software/Arm%20Virtual%20Hardware) for local execution

### Packs

- Required packs are listed in the file `retarget.csolution.yml`

## Project Structure

The project is generated using the [CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/) and
is defined in [`csolution`](https://open-cmsis-pack.github.io/cmsis-toolbox/YML-Input-Format/) format:

- `retarget.csolution.yml` lists the required packs and defines the hardware target and build-types (along with the compiler).
- `retarget.cproject.yml` defines the source files and the software components.

## Build project in VS Code

[Arm Keil Studio Pack Extensions](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack) for VS Code
include all required tools and interfaces to build the `csolution` projects.

> - See [Arm Keil Studio for VS Code Extensions User Guide](https://mdk-packs.github.io/vscode-cmsis-solution-docs/index.html)
>   for more information about using the Keil Studio extensions.

When the project is opened in VS Code with Arm Keil Studio Extensions installed one must first ensure that the build tools are configured.
The `Arm Tools Environment Manager` should handle this automatically and create `vcpkg-configuration.json` with all the necessary tools listed.
Right-click anywhere in the workspace and select "Configure Arm Tools Environment" to modify tools configuration. See
[Arm Tools Environment Manager Overview](https://marketplace.visualstudio.com/items?itemName=Arm.environment-manager) for available functionality.

To build a project using Keil Studio extensions open [CMSIS view](https://mdk-packs.github.io/vscode-cmsis-solution-docs/userinterface.html),
open "Manage Solution Settings" view and verify that "Active Target", "Active Projects" and "Build Type" are selected. Since there is only
one option in each category they should all be automatically selected.

You can execute the build by [selecting "Build"](https://mdk-packs.github.io/vscode-cmsis-solution-docs/userinterface.html#3-actions-available-through-the-cmsis-view) from the CMSIS view.

## Build Project from the Command Line

[CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/) provides [cbuild](https://open-cmsis-pack.github.io/cmsis-toolbox/build-tools/)
command-line tool that builds `csolution` projects.

The `retarget.csolution.yml` solution files defines `Debug` build-type and `VHT_CM3` target-type. To build this configuration
using Arm Compiler 6 toolchain execute the following command:

```bash
cbuild retarget.csolution.yml --toolchain AC6 --packs --context retarget.Debug+VHT_CM3
```

> **Note**
>
> - During the build process required packs may be downloaded (`--pack` flag).

By default the project does not specify the toolchain and one can build the project by specifying the AC6, GCC, CLANG or IAR toolchain.

## Execute Project

The project is configured for execution on Arm Virtual Hardware which removes the requirement for a physical hardware board.

To execute application image on Arm Virtual Hardware use below command:

```bash
FVP_MPS2_Cortex-M3 -f fvp-config.txt out/retarget/VHT_CM3/Debug/retarget.axf
```

> **Note**
>
> - For GCC and CLANG, the application image file has extension `.elf` and extension `.out` for IAR.

This command will execute the simulation model and in the default terminal window you should see the following output:

```
Starting ...
Hello World 1
Hello World 2
Hello World 3
Hello World 4
Hello World 5
Hello World 6
Hello World 7
Hello World 8
Hello World 9
Hello World 10

Finished
```

The simulation model will automatically terminate after the above output.
