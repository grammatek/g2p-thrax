# Thrax g2p

This project provides a grapheme-to-phoneme (g2p) tool based on Thrax-compiled g2p grammars. The grammar provided with
this project is for Icelandic g2p, following these  [transcription rules](Pronunciation_SAMPA.pdf) for X-SAMPA.
The file also gives an overview of the Icelandic sound system and pronunciation variants.

The tool is used as a command-line tool and word inputs can be provided either as a command line option, interactively
or as an input file.

This project aims also to easily compile all prerequisites by a Dockerfile.

# Prerequisites
## OpenFST + Thrax

OpenFST + Thrax have to be installed on the compilation host system.

- OpenFST library can be found at http://www.openfst.org/twiki/bin/view/FST/WebHome. Version 1.7.7 should be used.
- Thrax can be found at http://www.opengrm.org/twiki/bin/view/GRM/ThraxDownload. Version 1.3.3 should be used.

### Compilation + Installation
Note, that currently patching of the Configuration files in both projects have to be done to cleanly compile them.
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
The command line parameters are compatible with the usual Thrax/OpenFST command line parameter handling. The
additional option '--word_file' takes a text file as argument. This file should contain a list of words (tokens)
separated by newline. The token list will be transcribed and written to stdout.

Additionally, the grammar file has to be provided to the thraxg2p tool by the option `--far`.
The default is to search for the file [g2p.far](g2p.far) in the current working directory. This binary file is in the format
of an OpenFST archive and provided together with this project. It's the compiled grammar of [g2p.grm](g2p.grm).

# Limitations
The tool is not suitable for compound words, often found in Icelandic language. Therefore, splitting of compounds is
required as a pre processing step.

# Testing
Run `make test` inside the build directory for running the tests of the project. The tests are meant especially to
verify, that the grammar of [g2p.far](g2p.far) matches the test word list in [test_transcripts.txt](tests/test_transcripts.txt).

# License
This project can be used under the terms of the Apache-2 license. Please refer to [License.txt](License.txt) for exact
Copyright information.

Most of the source code of `RewriteTesterUtils.[h,cpp]` is blatantly copied from the Thrax command-line rewrite tester
(https://github.com/mjansche/thrax/blob/master/src/bin/rewrite-tester-utils.cc), and has been adapted to support word
lists provided by the command-line argument `--word_file`.