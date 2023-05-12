#include "Common.h"

int main(int argc, char* argv[])
{
    auto cmd = 'A';
    if (argc > 1) cmd = argv[1][0];

    if (cmd == 'A' || cmd == 'a') return main_AngleApp();
    if (cmd == 'R' || cmd == 'r') return main_RangeApp();

    printf("unknown command '%c'! exiting...", cmd);
    return -1;
}