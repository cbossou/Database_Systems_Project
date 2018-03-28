TEXCMP = latexmk -pdf
TEXSRC = icherdak_rebuttal.tex
TEXPDF = $(TEXSRC:%.tex=%.pdf)
TEXOUT = $(TEXPDF) *.out *.aux *.fls *.log *.fdb_latexmk *.bak *.bbl *.blg
OPENPDF = evince
REMOVE = rm -f
SPELLCHECK = aspell check

run: all
	$(OPENPDF) $(TEXPDF)

all: $(TEXPDF)

$(TEXPDF): $(TEXSRC)
	$(SPELLCHECK) $^
	$(TEXCMP) $^

clean:
	$(REMOVE) $(TEXOUT)

.PHONY: run all clean
