# Thrax g2p

This project provides the Thrax g2p (grapheme to phoneme) tool to convert Icelandic words into phonetic transcription.

The tool is used as a command-line tool and possible word inputs are either via command line option or via file input.

Most of the source code is blatently copied from the Thrax project of (...?) subdirectory and has been adapted to support word lists provided by command-line argument `--word_file`, and also to be easily compilable.


# Prerequisites
## OpenFST + Thrax

OpenFST + Thrax have to installed on the compilation host system.

- OpenFST library can be found at http://www.openfst.org/twiki/bin/view/FST/WebHome. Version 1.7.7 should be used.
- Thrax can be found at http://www.opengrm.org/twiki/bin/view/GRM/ThraxDownload. Version 1.3.3 should be used.

### Compilation + Installation
Please refer to the Dockerfile for the exact execution steps of compiling all prerequisites.

## Boost

The module Boost::locale is used for UTF-8 text normalization and Boost::filesystem for some file treatment.

# Compilation
This project uses CMake for compilation. If you have installed all prerequisites at standard paths, just do:

```
mkdir build && cd build && cmake .. && make
```

If you have installed the prerequisites at non-standard paths, use the following CMake options:

```
mkdir build && cd build && cmake -DCMAKE_PREFIX_PATH=<your root folder for installed packages> .. && make
``` 

# Execution
The command line parameters are compatible with the usual Thrax/OpenFST command line parameter handling. There is an additional option `--word_file` that is added to outputting bulk lists of Icelandic word to phoneme transcripts. This file needs to have each word separated via a newline.
Additionally, the grammar file has to be provided to the thraxg2p tool by the option `--far`. The default is to search for a file "g2p.far" in the current working directory.

# Testing
Run `make test` inside the build directory for running the unit tests of the project.