# I/O {#rt_template_io}

 - \subpage stderr_USART_c
 - \subpage stderr_user_c
 - \subpage stdin_USART_c
 - \subpage stdin_user_c
 - \subpage stdout_USART_c
 - \subpage stdout_user_c
 - \subpage tty_user_c

\page stderr_USART_c STDERR via USART

> **Note**
> - To have the full flexibility for retargeting different channels to different target hardware, each USART user code template contains a function to initialize the USART (here: `stderr_init`). When using the same hardware for multiple channels, the initialization functions need to be merged in one single function. Call this function during the initialization of the device's peripherals.

\include /io/stderr_USART.c

\page stderr_user_c STDERR User Template

\include /io/stderr_user.c

\page stdin_USART_c STDIN via USART

> **Note** 
> - To have the full flexibility for retargeting different channels to different target hardware, each USART user code template contains a function to initialize the USART (here `stdin_init`). When using the same hardware for multiple channels, the initialization functions need to be merged in one single function. Call this function during the initialization of the device's peripherals.

\include /io/stdin_USART.c

\page stdin_user_c STDIN User Template

\include /io/stdin_user.c

\page stdout_USART_c STDOUT via USART

> **Note**
> - To have the full flexibility for retargeting different channels to different target hardware, each USART user code template contains a function to initialize the USART (here: `stdout_init`). When using the same hardware for multiple channels, the initialization functions need to be merged in one single function. Call this function during the initialization of the device's peripherals.

\include /io/stdout_USART.c

\page stdout_user_c STDOUT User Template

\include /io/stdout_user.c

\page tty_user_c TTY User Template

\include /io/tty_user.c
