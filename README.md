# CMSIS-Compiler Support

## About

The software component **Compiler** allows you to retarget I/O functions of the standard C run-time library based on the C compiler that you are using in your project.

## Repository top level structure

```txt
    📦
    ┣ 📂 .github          GitHub Action workflow and configuration
    ┣ 📂 documentation    Target directory for generated documentation
    ┣ 📂 doxygen          Source directory for documentation
    ┣ 📂 include          Include header files of software components
    ┣ 📂 source           Source code of software components
    ┗ 📂 template         User code templates
```

## Generating Software Pack

Some helper scripts are provided to generate the release artifacts from this repository.

### Doxygen Documentation

Generating the HTML-formatted documentation from its Doxygen-based source is done via

```bash
CMSIS-Comiler $ ./DoxyGen/gen_doc.sh
```

Prerequisites for this script to succeed are:
 - Doxygen 1.9.2

### CMSIS-Pack Bundle

The CMSIS-Pack bundle can be generated with

```bash
CMSIS-Comiler $ ./gen_pack.sh
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

The version numbers are taken from the available tags. The shown release dates and
changelogs are one of:

1. For annotated tags the tagger date and the associated message is used.
2. For simple tags the committer date and message of the pointed-to commit is used.

## License

CMSIS-Comiler is licensed under Apache 2.0.

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
