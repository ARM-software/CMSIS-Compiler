# OS Interface {#rt_template_os_interface}

\ref os_interface_api provides **Custom** component that can be used to implement its functionality using code templates below.

Exposed OS Interface API is toolchain specific therefore make sure to select correct code template:

 - \ref retarget_os_c_armcc
 - \ref  retarget_os_gcc

## Arm Compiler {#retarget_os_c_armcc}

\include /os_interface/armcc/retarget_os.c


## GCC Newlib {#retarget_os_gcc}

**Code template used to retarget locking routines**

\include /os_interface/gcc/retarget_lock.c

**Code template used to retarget system calls**

> **Note**
> - All system call functions are provided in this template although there may not be necessary to reimplement them all. Functions that are not necessary may be removed or kept depending on the linker settings.
> - Reimplementing functions like `_open`, `_close`,` _write`, etc. breaks compatibility with the CMSIS-Compiler:IO component. Make sure to remove them from custom implementation when application does not require to reimplement them.

\include /os_interface/gcc/retarget_syscalls.c
