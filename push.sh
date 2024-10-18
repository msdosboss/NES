#!/bin/bash

echo "$1"

if [ "$1" == "cloud" ]; then
	echo "cloud active"
	echo "pacman.log\npacman.nes" > .gitignore
	git push -u cloud master
	echo "" > .gitignore
else
	git push -u origin master
fi
