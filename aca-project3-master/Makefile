SRCFILES := $(wildcard src/*.c)
HFILES := $(wildcard src/*.h)

all: branchsim

branchsim: $(SRCFILES) $(HFILES)
	gcc -Wall -g -o branchsim $(SRCFILES) -lm

submission: branchsim
	./bin/makesubmission.sh

grade: branchsim
	./bin/run_grader.py --fast

grade-full: branchsim
	./bin/run_grader.py

clean:
	rm -rfv test_results branchsim *-project3.tar.gz

.PHONY: all submission clean grade grade-full
