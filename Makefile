all: peru

gui/Makefile:
	cd gui && qmake Peru.pro && ./fixmake

gui/peru_sv.qm: gui/peru_sv.ts
	cd gui && lrelease peru_sv.ts

peru: gui/peru_sv.qm gui/Makefile
	make -C gui

clean:
	cd gui && make clean && rm -f *.qm
