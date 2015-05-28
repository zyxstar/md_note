
show:
	python show_doc.py

readme:
	python gen_readme.py

pull:
	git pull origin master:master

push:
	git add *;\
	git commit -m"update doc";\
	git push origin master:master;\
	git push github master:master




