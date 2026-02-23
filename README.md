[![License](https://img.shields.io/badge/License-Apache--2.0-green)](https://github.com/Arm-Software/CMSIS-Compiler/blob/main/LICENSE)
[![Usage Example](https://img.shields.io/github/actions/workflow/status/Arm-Software/CMSIS-Compiler/example.yml?logo=arm&logoColor=0091bd&label=Usage%20Example)](./.github/workflows/example.yml)
[![Build pack](https://img.shields.io/github/actions/workflow/status/Arm-Software/CMSIS-Compiler/pack.yml?logo=arm&logoColor=0091bd&label=Build%20pack)](./.github/workflows/pack.yml)
# CMSIS-Compiler Support

## About

**CMSIS-Compiler** software components allow embedded software developers to retarget standard I/O streams and
file operations to specific platform and enable thread safe operations using RTOS interface.

[CMSIS-Compiler documentation](https://arm-software.github.io/CMSIS-Compiler) explains available functionality and APIs.

> **Note**
> - CMSIS-Compiler replaces and extends retargeting functionality previously provided as part of *Keil::ARM_Compiler* pack.
> - See [Migrating projects from CMSIS v5 to CMSIS v6](https://learn.arm.com/learning-paths/microcontrollers/project-migration-cmsis-v6) for a guidance on updating existing projects to CMSIS-Compiler.

## Repository top level structure

```txt
    📦
    ┣ 📂 .github          GitHub Action workflow and configuration
    ┣ 📂 documentation    Documentation directory
    ┣ 📂 example          Usage examples
    ┣ 📂 include          Include header files of software components
    ┣ 📂 source           Source code of software components
    ┗ 📂 template         User code templates
```

## Generating Software Pack

Some helper scripts are provided to generate the release artifacts from this repository.

### Doxygen Documentation

Generating the HTML-formatted documentation from its Doxygen-based source is done via

```sh
CMSIS-Compiler $ ./documentation/doxygen/gen_doc.sh
```

Prerequisites for this script to succeed are:

- Doxygen 1.9.6

Also see [Documentation README](./documentation/README.md).

### CMSIS-Pack Bundle

The CMSIS-Pack bundle can be generated with

```sh
CMSIS-Compiler $ ./gen_pack.sh
```

Prerequisites for this script to succeed are:

- Generated documentation (see above)
- 7z (or GNU zip)
- packchk
- xmllint (optional)

### Version and Changelog Inference

The version and changelog embedded into the documentation and pack are inferred from the
local Git history. In order to get the full changelog one needs to have a full clone (not
a shallow one) including all release tags.

The version numbers and change logs are taken from the available annotated tags.

### Release Pack

A release is simply done via the GitHub Web UI. The newly created tag needs to have
the pattern `v<version>` where `<version>` shall be the SemVer `<major>.<minor>.<patch>`
version string for the release. The release description is used as the change log
message for the release.

When using an auto-generated tag (via Web UI) the release description is used as the
annotation message for the generated tag. Alternatively, one can prepare the release
tag in the local clone and add the annotation message independently from creating the
release.

Once the release is published via the GitHub Web UI the release workflow generates the
documentation and the pack (see above) and attaches the resulting pack archive as an
additional asset to the release.

## License Terms

CMSIS-Compiler is licensed under [Apache License 2.0](LICENSE).

### Note

Individual files contain the following tag instead of the full license text.

SPDX-License-Identifier: Apache-2.0

This enables machine processing of license information based on the SPDX License Identifiers that are here available: http://spdx.org/licenses/

## Contributions and Pull Requests

Contributions are accepted under Apache 2.0. Only submit contributions where you have authored all of the code.

### Issues, Labels

Please feel free to raise an issue on GitHub
to report misbehavior (i.e. bugs)

Issues are your best way to interact directly with the maintenance team and the community.
We encourage you to append implementation suggestions as this helps to decrease the
workload of the very limited maintenance team.

We shall be monitoring and responding to issues as best we can.
Please attempt to avoid filing duplicates of open or closed items when possible.
In the spirit of openness we shall be tagging issues with the following:

- **bug** – We consider this issue to be a bug that shall be investigated.

- **wontfix** - We appreciate this issue but decided not to change the current behavior.

- **out-of-scope** - We consider this issue loosely related to CMSIS. It might be implemented outside of CMSIS. Let us know about your work.

- **question** – We have further questions about this issue. Please review and provide feedback.

- **documentation** - This issue is a documentation flaw that shall be improved in the future.

- **DONE** - We consider this issue as resolved - please review and close it. In case of no further activity, these issues shall be closed after a week.

- **duplicate** - This issue is already addressed elsewhere, see a comment with provided references.
