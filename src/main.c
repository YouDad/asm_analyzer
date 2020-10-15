#include "common.h"
#include "handlers.h"

int main()
{
	char cmd[128];

	printf("<luwh0708@thundersoft.com>\n$ ");
	while (1) {
		if (fgets(cmd, sizeof(cmd), stdin) == 0) {
			// catch EOF(Ctrl+D)
			break;
		}

		int ret = dispatch(cmd);
		if (ret) {
			return ret;
		}

		printf("\n$ ");
	}

	return 0;
}
