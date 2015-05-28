
show:
	python show_doc.py $(path)

readme:
	python gen_readme.py

pull:
	git pull origin master:master


.ONESHELL:
push:
	git add *
	git commit -m"update doc"
	git push origin master:master
	git push github master:master




