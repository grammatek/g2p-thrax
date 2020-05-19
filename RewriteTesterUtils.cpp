//
// Created by Daniel Schnell on 19.05.20.
//

#include "RewriteTesterUtils.h"
#include <thrax/compat/utils.h>
#include <fst/arc.h>
#include <fst/flags.h>
#include <fst/fst.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>
#include <thrax/grm-manager.h>
#include <thrax/algo/paths.h>
#include <thrax/symbols.h>

using ::fst::StdArc;
using ::fst::StdVectorFst;
using ::fst::StringCompiler;
using ::fst::StringTokenType;
using ::fst::SymbolTable;
using ::thrax::FstToStrings;
using ::thrax::GetGeneratedSymbolTable;
using ::thrax::RuleTriple;

#define HISTORY_FILE ".rewrite-tester-history"

DEFINE_string(far, "", "Path to the FAR.");
DEFINE_string(rules, "", "Names of the rewrite rules.");
DEFINE_string(input_mode, "byte", "Either \"byte\", \"utf8\", or the path to a "
                                  "symbol table for input parsing.");
DEFINE_string(output_mode, "byte", "Either \"byte\", \"utf8\", or the path to "
                                   "a symbol table for input parsing.");
DEFINE_string(history_file, HISTORY_FILE,
              "Location of history file");
DEFINE_int64(noutput, 1, "Maximum number of output strings for each input.");
DEFINE_bool(show_details, false, "Show the output of each individual rule when"
                                 " multiple rules are specified.");
DEFINE_string(field_separator, " ",
              "Field separator for strings of symbols from a symbol table.");

DEFINE_string(word_file, " ",
              "File with newline separated fields that should be used for input");

namespace thrax {
    namespace {

        using ::fst::kNoStateId;
        using ::fst::LabelsToUTF8String;
        using ::fst::PathIterator;
        using ::fst::Project;
        using ::fst::PROJECT_OUTPUT;
        using ::fst::RmEpsilon;
        using ::fst::ShortestPath;
        using ::fst::StdArc;
        using ::fst::StdVectorFst;
        using ::fst::SymbolTable;

        using Label = StdArc::Label;

        inline bool AppendLabel(Label label, TokenType type,
                                const SymbolTable *generated_symtab,
                                SymbolTable *symtab, std::string *path) {
            if (label != 0) {
                // Check first to see if this label is in the generated symbol set. Note
                // that this should not conflict with a user-provided symbol table since
                // the parser used by GrmCompiler doesn't generate extra labels if a
                // string is parsed using a user-provided symbol table.
                if (generated_symtab && !generated_symtab->Find(label).empty()) {
                    const auto &sym = generated_symtab->Find(label);
                    *path += "[" + sym + "]";
                } else if (type == SYMBOL) {
                    const auto &sym = symtab->Find(label);
                    if (sym.empty()) {
                        LOG(ERROR) << "Missing symbol in symbol table for id: " << label;
                        return false;
                    }
                    // For non-byte, non-UTF8 symbols, one overwhelmingly wants these to be
                    // space-separated.
                    if (!path->empty()) *path += FLAGS_field_separator;
                    *path += sym;
                } else if (type == BYTE) {
                    path->push_back(label);
                } else if (type == UTF8) {
                    std::string utf8_string;
                    std::vector<Label> labels;
                    labels.push_back(label);
                    if (!LabelsToUTF8String(labels, &utf8_string)) {
                        LOG(ERROR) << "LabelsToUTF8String: Bad code point: " << label;
                        return false;
                    }
                    *path += utf8_string;
                }
            }
            return true;
        }

    }  // namespace

    bool FstToStrings(const StdVectorFst &fst,
                      std::vector<std::pair<std::string, float>> *strings,
                      const SymbolTable *generated_symtab, TokenType type,
                      SymbolTable *symtab, size_t n) {
        StdVectorFst shortest_path;
        if (n == 1) {
            ShortestPath(fst, &shortest_path, n);
        } else {
            // The uniqueness feature of ShortestPath requires us to have an acceptor,
            // so we project and remove epsilon arcs.
            StdVectorFst temp(fst);
            Project(&temp, PROJECT_OUTPUT);
            RmEpsilon(&temp);
            ShortestPath(temp, &shortest_path, n, /*unique=*/true);
        }
        if (shortest_path.Start() == kNoStateId) return false;
        for (PathIterator<StdArc> iter(shortest_path, /*check_acyclic=*/false);
             !iter.Done(); iter.Next()) {
            std::string path;
            for (const auto label : iter.OLabels()) {
                if (!AppendLabel(label, type, generated_symtab, symtab, &path)) {
                    return false;
                }
            }
            strings->emplace_back(std::move(path), iter.Weight().Value());
        }
        return true;
    }

    const SymbolTable *GetGeneratedSymbolTable(GrmManagerSpec<StdArc> *grm) {
        const auto *symbolfst = grm->GetFst("*StringFstSymbolTable");
        return symbolfst ? symbolfst->InputSymbols()->Copy() : nullptr;
    }

}  // namespace thrax


RewriteTesterUtils::RewriteTesterUtils() :
        compiler_(nullptr),
        byte_symtab_(nullptr),
        utf8_symtab_(nullptr),
        input_symtab_(nullptr),
        output_symtab_(nullptr)  { }

RewriteTesterUtils::~RewriteTesterUtils() {
    delete compiler_;
    delete input_symtab_;
    delete output_symtab_;
    delete byte_symtab_;
    delete utf8_symtab_;
}

void RewriteTesterUtils::Initialize() {
    CHECK(grm_.LoadArchive(FLAGS_far));
    rules_ = ::fst::StringSplit(FLAGS_rules, ',');
    byte_symtab_ = nullptr;
    utf8_symtab_ = nullptr;
    if (rules_.empty()) LOG(FATAL) << "--rules must be specified";
    for (size_t i = 0; i < rules_.size(); ++i) {
        RuleTriple triple(rules_[i]);
        const auto *fst = grm_.GetFst(triple.main_rule);
        if (!fst) {
            LOG(FATAL) << "grm.GetFst() must be non nullptr for rule: "
                       << triple.main_rule;
        }
        StdVectorFst vfst(*fst);
        // If the input transducers in the FAR have symbol tables then we need to
        // add the appropriate symbol table(s) to the input strings, according to
        // the parse mode.
        if (vfst.InputSymbols()) {
            if (!byte_symtab_ &&
                vfst.InputSymbols()->Name() ==
                ::thrax::function::kByteSymbolTableName) {
                byte_symtab_ = vfst.InputSymbols()->Copy();
            } else if (!utf8_symtab_ &&
                       vfst.InputSymbols()->Name() ==
                       ::thrax::function::kUtf8SymbolTableName) {
                utf8_symtab_ = vfst.InputSymbols()->Copy();
            }
        }
        if (!triple.pdt_parens_rule.empty()) {
            fst = grm_.GetFst(triple.pdt_parens_rule);
            if (!fst) {
                LOG(FATAL) << "grm.GetFst() must be non nullptr for rule: "
                           << triple.pdt_parens_rule;
            }
        }
        if (!triple.mpdt_assignments_rule.empty()) {
            fst = grm_.GetFst(triple.mpdt_assignments_rule);
            if (!fst) {
                LOG(FATAL) << "grm.GetFst() must be non nullptr for rule: "
                           << triple.mpdt_assignments_rule;
            }
        }
    }
    generated_symtab_ = GetGeneratedSymbolTable(&grm_);
    if (FLAGS_input_mode == "byte") {
        compiler_ = new StringCompiler<StdArc>(StringTokenType::BYTE);
    } else if (FLAGS_input_mode == "utf8") {
        compiler_ = new StringCompiler<StdArc>(StringTokenType::UTF8);
    } else {
        input_symtab_ = SymbolTable::ReadText(FLAGS_input_mode);
        if (!input_symtab_) {
            LOG(FATAL) << "Invalid mode or symbol table path.";
        }
        compiler_ =
                new StringCompiler<StdArc>(StringTokenType::SYMBOL, input_symtab_);
    }
    output_symtab_ = nullptr;
    if (FLAGS_output_mode == "byte") {
        type_ = BYTE;
    } else if (FLAGS_output_mode == "utf8") {
        type_ = UTF8;
    } else {
        type_ = SYMBOL;
        output_symtab_ = SymbolTable::ReadText(FLAGS_output_mode);
        if (!output_symtab_) {
            LOG(FATAL) << "Invalid mode or symbol table path.";
        }
    }
}

const std::string RewriteTesterUtils::ProcessInput(const std::string& input,
                                                   bool prepend_output) {
    StdVectorFst input_fst;
    StdVectorFst output_fst;
    if (!compiler_->operator()(input, &input_fst)) {
        return "Unable to parse input string.";
    }
    std::ostringstream sstrm;
    // Set symbols for the input, if appropriate
    if (byte_symtab_ && type_ == BYTE) {
        input_fst.SetInputSymbols(byte_symtab_);
        input_fst.SetOutputSymbols(byte_symtab_);
    } else if (utf8_symtab_ && type_ == UTF8) {
        input_fst.SetInputSymbols(utf8_symtab_);
        input_fst.SetOutputSymbols(utf8_symtab_);
    } else if (input_symtab_ && type_ == SYMBOL) {
        input_fst.SetInputSymbols(input_symtab_);
        input_fst.SetOutputSymbols(input_symtab_);
    }
    bool succeeded = true;
    for (size_t i = 0; i < rules_.size(); ++i) {
        RuleTriple triple(rules_[i]);
        if (grm_.Rewrite(triple.main_rule, input_fst, &output_fst,
                         triple.pdt_parens_rule, triple.mpdt_assignments_rule)) {
            if (FLAGS_show_details && rules_.size() > 1) {
                std::vector<std::pair<std::string, float>> tmp;
                FstToStrings(output_fst, &tmp, generated_symtab_, type_,
                             output_symtab_, FLAGS_noutput);
                for (const auto& one_result : tmp) {
                    sstrm << "output of rule[" << triple.main_rule
                          << "] is: " << one_result.first << '\n';
                }
            }
            input_fst = output_fst;
        } else {
            succeeded = false;
            break;
        }
    }
    std::vector<std::pair<std::string, float>> strings;
    std::set<std::string> seen;
    if (succeeded && FstToStrings(output_fst, &strings,
                                  generated_symtab_, type_,
                                  output_symtab_, FLAGS_noutput)) {
        for (auto it = strings.cbegin(); it != strings.cend(); ++it) {
            const auto sx = seen.find(it->first);
            if (sx != seen.end()) continue;
            if (prepend_output) {
                sstrm << "Output string: " << it->first;
            } else {
                sstrm << it->first;
            }
            if (FLAGS_noutput != 1 && it->second != 0) {
                sstrm << " <cost=" << it->second << '>';
            }
            seen.insert(it->first);
            if (it + 1 != strings.cend()) sstrm << '\n';
        }
        return sstrm.str();
    } else {
        return "Rewrite failed.";
    }
}

// Run() for interactive mode.
void RewriteTesterUtils::Run()
{
    if (FLAGS_word_file != " ")
    {
        std::ifstream file(FLAGS_word_file);
        std::string   line;
        while(std::getline(file, line))
        {
            std::cout << line << "\t" << ProcessInput(line, false) << std::endl;
        }
    }
    else
    {
        std::string input;
        while (ReadInput(&input)) std::cout << ProcessInput(input) << std::endl;
    }
}

bool RewriteTesterUtils::ReadInput(std::string* s) {
    std::cout << "Input string: ";
    return static_cast<bool>(getline(std::cin, *s));
}
