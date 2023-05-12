#include "Common.h"

int main(int argc, char* argv[])
{

#if defined(_RANGE_API)
	#define _cmd 'R';
#elif defined(_ANGLE_API)
	#define _cmd 'A';
#endif

	auto cmd = _cmd;
	if (argc > 1) cmd = argv[1][0];

	if (cmd == 'A' || cmd == 'a') return main_AngleApp();
	if (cmd == 'R' || cmd == 'r') return main_RangeApp();

	printf("unknown command '%c'! exiting...", cmd);
	return -1;
}