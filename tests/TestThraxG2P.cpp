#include <regex>
#include "RewriteTesterUtils.h"

#ifdef CHECK
#   undef CHECK
#endif

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

DEFINE_string(test_file, "",
              "File with newline separated fields that should be used for testing");

int main(int argc, char** argv)
{
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();

    std::set_new_handler(FailedNewHandler);
    SET_FLAGS(argv[0], &argc, &argv, true);

    return RUN_ALL_TESTS(argc, argv);
}

TEST_GROUP(RewriteTesterUtils)
{
    std::unique_ptr<RewriteTesterUtils> utils;

    void setup()
    {
        utils = std::make_unique<RewriteTesterUtils>();
        utils->Initialize();
    }
    void teardown()
    {
        utils = nullptr;
    }
};

TEST(RewriteTesterUtils, TestTranscripts)
{
    CHECK_FALSE(FLAGS_test_file.empty());
    std::ifstream file(FLAGS_test_file);
    std::string line;

    // input file has <tab> separated lists of tokens in the format
    // word<tab>phonemes, so we split it
    while(std::getline(file, line))
    {
        std::regex regex{R"(\t)"};
        std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
        std::vector<std::string> words{it, {}};
        CHECK(words.size() == 2);

        auto phonemes = utils->processWord(words[0]);
        CHECK(phonemes == words[1]);
    }
}

// EOF
