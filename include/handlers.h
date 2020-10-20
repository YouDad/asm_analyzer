#pragma once
struct route {
	char id[16];
	char small_id[3];
	int (* handler)(char *args);
	char desc[64];
	char desc2[64];
};

int dispatch(char cmd[128]);
