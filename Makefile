
all: peru

.PHONY: fixed_Makefile clean

gui/Makefile:
	cd gui && qmake Peru.pro

# Some versions of qmake added the no exceptions
# flag to the Makefile. But I want them, so be
# sure to remove the flag.
fixed_Makefile: gui/Makefile
	sed -i s/-fno-exceptions// gui/Makefile

peru_sv.qm: gui/peru_sv.ts
	cd gui && lrelease peru_sv.ts

peru: peru_sv.qm fixed_Makefile
	make -C gui

clean: fixed_Makefile
	cd gui && make clean && rm -f *.qm

