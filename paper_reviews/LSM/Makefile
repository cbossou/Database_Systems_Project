MODULE = icherdak_LSM
SRC = $(MODULE:%=%.tex)
PDF = $(MODULE:%=%.pdf)
OUT = $(PDF) *.aux *.fdb_latexmk *.out *.log *.fls

run: $(PDF)
	evince $^

$(PDF): $(SRC)
	latexmk -pdf $^

clean:
	rm -f $(OUT)

.PHONY: run
