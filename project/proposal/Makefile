TEXCMP = latexmk -pdf
TEXSRC = icherdak_proposal.tex
TEXPDF = $(TEXSRC:%.tex=%.pdf)
TEXOUT = $(TEXPDF) *.out *.aux *.fls *.log *.fdb_latexmk
OPENPDF = evince
REMOVE = rm -f

run: all
	$(OPENPDF) $(TEXPDF)

all: $(TEXPDF)

$(TEXPDF): $(TEXSRC)
	$(TEXCMP) $^

clean:
	$(REMOVE) $(TEXOUT)

.PHONY: run all clean
