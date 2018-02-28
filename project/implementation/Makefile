CPPSRCEXT = cc
CPPHEADEXT = h
MAIN = Main
MODULES = ExtraUtil BenchMark
HEADERS = $(MODULES:%=%.$(CPPHEADEXT))
SRC = $(MAIN:%=%.$(CPPSRCEXT)) $(MODULES:%=%.$(CPPSRCEXT))
OBJ = $(SRC:%.$(CPPSRCEXT)=%.o)
EXE = RocksBench
CPP = g++
CPPCOMP = -c
CPPLINK = -o

all: $(EXE)

$(EXE): $(OBJ)
	$(CPP) $(CPPLINKFLAGS) $(CPPLINK) $@ $^

%.o: %.$(CPPSRCEXT)
	$(CPP) $(CPPCOMPFLAGS) $(CPPCOMP) $^

clean:
	rm -f $(OBJ)

spotless: clean
	rm -f $(EXE)

.PHONY: all clean spotless
