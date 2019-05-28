
git-revert:
	git reset --hard HEAD

git-pull: 
	git pull

git-commit:
	git add -A && git commit -m '-' && git push

goto-github:
	open https://github.com/meg768/rpi-ws281x

goto-npm:
	open https://www.npmjs.com/package/rpi-ws281x

npm-publish:
	npm publish