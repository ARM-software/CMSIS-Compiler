# Multithreading Support Retarget {#rt_os}

**CMSIS-Compiler** enables the standard C library multithreading interface retargeting via **OS Interface** component.

## OS Interface {#rt_os_components}

The software component **CMSIS-Compiler:OS Interface** exposes \ref os_interface_api API that depends on the used toolchain i.e. on the standard C library embedded with the toolchain. Available components are implementations of this API and default components are as follows:

 Component       | Description
:----------------|:------------
 **CMSIS-RTOS2** | \ref os_interface_api implementation using CMSIS-RTOS2 API
 **Custom**      | Placeholder for custom \ref os_interface_api implementation

Default implementation uses **CMSIS-RTOS2** to implement multithreading support and enables easy retargeting to any RTOS that implements RTOS2 API.

**Custom** component is available to enable access to \ref os_interface_api API header file when application provides an custom implementation. Custom implementation is typically used when application uses RTOS native API.

To help you implement the functionality the OS Interface:Custom component also provides the \ref rt_template_os_interface "code templates".
