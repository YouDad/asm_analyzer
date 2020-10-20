#include "common.h"
#include "handlers.h"

int main(int argc, char **argv)
{
	char cmd[128] = "load ";
	if (argc == 2) {
		strcpy(&cmd[5], argv[1]);
	}

	printf("<luwh0708@thundersoft.com>\n");
	if (argc == 2) {
		dispatch(cmd);
	}
	while (1) {
		printf("\n$ ");
		if (fgets(cmd, sizeof(cmd), stdin) == 0) {
			// catch EOF(Ctrl+D)
			dispatch("exit");
			break;
		}

		int ret = dispatch(cmd);
		if (ret) {
			printf("failed return %d\n", ret);
		}
	}

	return 0;
}
