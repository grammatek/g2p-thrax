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
    uint32 phonemeTranscriptsErrors = 0;
    while(std::getline(file, line))
    {
        std::regex regex{R"(\t)"};
        std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
        std::vector<std::string> words{it, {}};

        CHECK(words.size() == 2);

        auto phonemes = utils->processWord(words[0]);
        if (phonemes != words[1])
        {
            // we want to see al false transcripts in the test output, therefore we don't bump out early
            printf("(%s):(%s):(%s)\n", words[0].c_str(), words[1].c_str(), phonemes.c_str());
            phonemeTranscriptsErrors++;
        }
    }
    if (phonemeTranscriptsErrors > 0)
    {
        printf("%d phoneme transcript errors\n", phonemeTranscriptsErrors);
    }
    CHECK(phonemeTranscriptsErrors == 0);
}

// EOF
