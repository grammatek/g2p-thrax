#include "RewriteTesterUtils.h"


int main(int argc, char **argv)
{
    std::set_new_handler(FailedNewHandler);
    SET_FLAGS(argv[0], &argc, &argv, true);

    RewriteTesterUtils utils;
    utils.Initialize();
    utils.Run();

    return 0;
}
