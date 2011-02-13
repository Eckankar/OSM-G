all: pdf ps

pdf: roadmap.pdf

ps: roadmap.ps

# This is a bit of a kludge, but it was the best I could come up with
# short of adding conditionals to the latex source and recompiling for
# monochrome
roadmap.ps: color.ps
	./remove-textcolor.pl <color.ps > roadmap.ps

color.ps: roadmap.dvi
	dvips -Ppdf roadmap.dvi -o color.ps

roadmap.pdf: color.ps
	ps2pdf14 color.ps roadmap.pdf

roadmap.dvi : roadmap.tex
	latex -interaction=errorstopmode roadmap.tex
	latex -interaction=errorstopmode roadmap.tex
	makeindex -s dotfill.ist roadmap
	latex -interaction=errorstopmode roadmap.tex
	latex -interaction=errorstopmode roadmap.tex

clean: distclean
	rm -f *.pdf *.ps

distclean:
	rm -f *~ *.aux *.log *.idx *.ilg *.ind *.out *.toc *.dvi color.ps
