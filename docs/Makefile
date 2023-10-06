NAME := dokumentace

build: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps $(NAME).pdf
	make clean

.PHONY: clean
clean:
	rm $(NAME).dvi
	rm $(NAME).aux
	rm $(NAME).log
	rm $(NAME).ps
	rm $(NAME).toc
