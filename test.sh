#!/bin/bash

for file in out/*.test; do
	$file
	ret=$?
	filename=${file#out/}
	filename=${filename%.test}
	if [[ "$ret" != "0" ]]; then
		echo -e "$filename: \033[31mfailed($ret)\033[0m"
		exit 1
	else
		echo -e "$filename: \033[32msuccess\033[0m"
	fi
done

echo -e "$0: \033[32mtest success\033[0m"
exit 0
