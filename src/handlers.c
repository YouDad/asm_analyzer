#include "common.h"
#include "handlers.h"

struct route routes[] = {
	{"help", "h", handle_help, "outputs help information"},
	{"load", "l", handle_load, "load file to analysis"},
};

int dispatch(char cmd[128])
{
	char keyword[128];
	char *args;

	sscanf(cmd, "%s", keyword);
	args = cmd + strlen(keyword) + 1;

	for (int i = 0; i < array_size(routes); i++) {
		struct route *p = routes + i;
		if (p->id[0] && strcmp(p->id, keyword) == 0) {
			return p->handler(args);
		}
		if (p->small_id[0] && strcmp(p->small_id, keyword) == 0) {
			return p->handler(args);
		}
	}

	return handle_help(args);
}

int handle_help(char *args)
{
	for (int i = 0; i < array_size(routes); i++) {
		printf("%s/%s: %s\n", routes[i].small_id, routes[i].id, routes[i].desc);
		if (routes[i].desc2[0]) {
			// TODO: some spaces
			printf("%s\n", routes[i].desc2);
		}
	}
	return 0;
}

int handle_load(char *args)
{
	char filename[128];

	sscanf(args, "%s", filename);
	printf("load file: %s\n", filename);
	return 0;
}
