all: kvant.pdf ../debug/kvant.pdf ../release/kvant.pdf

kvant.pdf: kvant.tex
	-pdflatex -interaction=batchmode $<
	-pdflatex -interaction=batchmode $<

../%/kvant.pdf: kvant.pdf
	if [ -d $(@D) ] ; then cp $< $@ ; fi
