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
CPPCOMPFLAGS = --std=c++11 -Wall -Werror -pedantic
CPPLINKFLAGS = -lboost_filesystem -lboost_system

all: $(EXE)

$(EXE): $(OBJ)
	$(CPP) $(CPPLINK) $@ $^ $(CPPLINKFLAGS)

%.o: %.$(CPPSRCEXT)
	$(CPP) $(CPPCOMPFLAGS) $(CPPCOMP) $^

clean:
	rm -f $(OBJ)

spotless: clean
	rm -f $(EXE)

.PHONY: all clean spotless
