all: zkvant.pdf ../debug/zkvant.pdf ../release/zkvant.pdf

kvant.pdf: zkvant.tex

%kvant.pdf: kvant.tex
	-pdflatex -interaction=batchmode $<
	-pdflatex -interaction=batchmode $<

../%/kvant.pdf: kvant.pdf
	if [ -d $(@D) ] ; then cp $< $@ ; fi
