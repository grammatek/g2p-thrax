# Thrax g2p

This project provides a grapheme-to-phoneme (g2p) tool based on Thrax-compiled g2p grammars. The grammar provided with this project is for Icelandic g2p, following the transcription rules in Icelandic_pronunciation_rules.pdf. The file also gives an overview of the Icelandic sound system and pronunciation variants.

The tool is used as a command-line tool and possible word inputs are either via command line option or via file input.

Most of the source code is blatently copied from the Thrax command-line rewrite tester (https://github.com/mjansche/thrax/blob/master/src/bin/rewrite-tester-utils.cc), and has been adapted to support word lists provided by command-line argument `--word_file`, and also to be easily compilable.


# Prerequisites
## OpenFST + Thrax

OpenFST + Thrax have to be installed on the compilation host system.

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
The command line parameters are compatible with the usual Thrax/OpenFST command line parameter handling. There is an additional option '--word_file' that takes a text file as argument, the file should contain a list of words (tokens) separated by newline. The token list will be transcribed and written to stdout.
Additionally, the grammar file has to be provided to the thraxg2p tool by the option `--far`. The default is to search for a file "g2p.far" in the current working directory.

# Testing
Run `make test` inside the build directory for running the unit tests of the project.