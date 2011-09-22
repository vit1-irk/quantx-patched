FILE=kvant
.SUFFIXES: .pdf
all: $(FILE).pdf
all: ../debug/$(FILE).pdf
all: ../release/$(FILE).pdf

$(FILE).pdf: $(FILE).tex
	-pdflatex -interaction=batchmode $<
	pdflatex -interaction=batchmode $<

../%/$(FILE).pdf: $(FILE).pdf
	if [ -d $(@D) ] ; then cp $< $@ ; fi
