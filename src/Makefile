CFLAGS=-Wall -g

default: all
include ../make/arch.mk
LINKLIBS := -ldl
SRC=.
TARGETs = dirs
TARGETs += $(DBIN)/cons-test
TARGETs += $(DBIN)/box-test 
TARGETs += $(DBIN)/sexp-parse-test 
TARGETs += $(DBIN)/interp-test
TARGETs += $(DBIN)/interp-program
all: $(TARGETs)

$(DBIN)/%-test: $(DOBJ)/%-test.o $(DOBJ)/%.o
	gcc -o $@ $^ $(LINKLIBS)
$(DOBJ)/%-test.o: $(SRC)/%-test.c $(SRC)/%.h
	gcc -o $@ -c $< $(CFLAGS)
$(DOBJ)%.o: $(SRC)/%.c $(SRC)/%.h
	gcc -o $@ -c $< $(CFLAGS)

$(DBIN)/box-test: $(DOBJ)/box-test.o $(DOBJ)/box.o $(DOBJ)/cons.o
	gcc -o $@ $^ $(LINKLIBS)

$(DBIN)/sexp-parse-test: $(DOBJ)/sexp-parse-test.o $(DOBJ)/sexp-parse.o $(DOBJ)/box.o $(DOBJ)/cons.o
	gcc -o $@ $^ $(LINKLIBS)

$(DBIN)/interp-test: $(DOBJ)/interp-test.o $(DOBJ)/specials.o $(DOBJ)/interp.o $(DOBJ)/sexp-parse.o $(DOBJ)/box.o $(DOBJ)/cons.o
	gcc -o $@ $^ $(LINKLIBS)

$(DBIN)/interp-program: $(DOBJ)/interp-program.o $(DOBJ)/specials.o $(DOBJ)/interp.o $(DOBJ)/sexp-parse.o $(DOBJ)/box.o $(DOBJ)/cons.o
	gcc -o $@ $^ $(LINKLIBS)

# $(DBIN)/sexp-test: $(DOBJ)/sexp-test.o $(DOBJ)/sexp.o $(DOBJ)/specials.o $(DOBJ)/interp.o $(DOBJ)/sexp-parse.o $(DOBJ)/box.o $(DOBJ)/cons.o
# 	gcc -o $@ $^ $(LINKLIBS)

clean:
	-rm -r $(BUILDDIR)
