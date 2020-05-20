//
// Created by Daniel Schnell on 19.05.20.
//

#ifndef SIM_G2P_REWRITETESTERUTILS_H
#define SIM_G2P_REWRITETESTERUTILS_H

#include <string>
#include <vector>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <thrax/grm-manager.h>

enum TokenType { SYMBOL = 1, BYTE = 2, UTF8 = 3 };

namespace thrax {

// Computes the n-shortest paths and returns a vector of strings, each string
// corresponding to each path. The mapping of labels to strings is controlled by
// the type and the symtab. Elements that are in the generated label set from
// the grammar are output as "[name]" where "name" is the name of the generated
// label. Paths are sorted in ascending order of weights.

    bool FstToStrings(const ::fst::VectorFst<::fst::StdArc> &fst,
                      std::vector<std::pair<std::string, float>> *strings,
                      const ::fst::SymbolTable *generated_symtab,
                      TokenType type = BYTE,
                      ::fst::SymbolTable *symtab = nullptr, size_t n = 1);

// Find the generated labels from the grammar.

    const ::fst::SymbolTable *GetGeneratedSymbolTable(
            GrmManagerSpec<::fst::StdArc> *grm);

}  // namespace thrax



class RewriteTesterUtils {
public:
    RewriteTesterUtils();

    ~RewriteTesterUtils();

    void Initialize();

    void Run();

    // Runs the input through the FSTs. Prepends "Output string:" to each line if
    // prepend_output is true
    const std::string ProcessInput(const std::string& input,
                                   bool prepend_output = true);

private:
    // Reader for the input in interactive version.
    bool ReadInput(std::string* s);

    ::thrax::GrmManagerSpec<::fst::StdArc> grm_;
    std::vector<std::string> rules_;
    ::fst::StringCompiler<::fst::StdArc>* compiler_;
    ::fst::SymbolTable* byte_symtab_;
    ::fst::SymbolTable* utf8_symtab_;
    const ::fst::SymbolTable* generated_symtab_;
    ::fst::SymbolTable* input_symtab_;
    TokenType type_;
    ::fst::SymbolTable* output_symtab_;

    RewriteTesterUtils(const RewriteTesterUtils&) = delete;
    RewriteTesterUtils& operator=(const RewriteTesterUtils&) = delete;
};


#endif //SIM_G2P_REWRITETESTERUTILS_H
