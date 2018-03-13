MODULE = icherdak_dcc
SRC = $(MODULE:%=%.tex)
PDF = $(MODULE:%=%.pdf)
OUT = $(PDF) *.aux *.fdb_latexmk *.out *.log *.fls *.toc

run: $(PDF)
	evince $^

$(PDF): $(SRC)
	latexmk -pdf $^

clean:
	rm -f $(OUT)

.PHONY: run
