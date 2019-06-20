MK=latexmk -xelatex -interaction=nonstopmode -halt-on-error -file-line-error

all: clean build

build:
	$(MK)

watch:
	$(MK) -pvc

clean:
	$(MK) -C

.PHONY: all watch build clean
