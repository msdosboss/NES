#!/bin/bash


if [ "$1" == "cloud" ]; then
	echo -e "pacman.log\npacman.nes" > .gitignore

	git add .gitignore
	git commit -m "Update .gitignore for cloud push"
	
	git push -u cloud master

	echo "" > .gitignore

	git add .gitignore
	git commit "Clear .gitignore after cloud push"
else
	git push -u origin master
fi
