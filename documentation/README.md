# CMSIS-Compiler Documentation

CMSIS-Compiler documentation in HTML format is published online at [https://arm-software.github.io/CMSIS-Compiler](https://arm-software.github.io/CMSIS-Compiler).

The version drop-down menu allows to switch between the documentation provided with official releases and the latest draft documentation for the main branch.

The documentation source is maintained in `/documentation/doxygen/` folder as a mixture of markdown and doxygen formats. Software source and header files, templates and examples may also contribute information that gets integrated into the final documentation.

Generating the HTML-formatted documentation from the source is done with `gen_doc.sh` script:

```sh
CMSIS-Compiler $ ./documentation/doxygen/gen_doc.sh
```

The script expects specific version of [doxygen](https://www.doxygen.nl/) to be installed locally. After successful execution the resulting documentation package is then available in `./documentation/html/`.
