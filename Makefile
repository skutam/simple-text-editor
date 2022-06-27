CFLAGS = -g -pedantic -ansi -std=c99 -O2 -Wall -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 `pkg-config --cflags gtk+-3.0`
LDLIBS = `pkg-config --libs gtk+-3.0`
LDFLAGS = -g
SOURCES := $(shell find -wholename './src/*.c')
OUT_ZIP_FILE=editor.zip


all : editor editor_run
	echo $(SOURCES)

editor_run:
	./editor

editor:
	cc $(CFLAGS) -o editor $(SOURCES) $(LDLIBS)

clean :
	rm -f editor.o editor

zip:
	zip -r ${OUT_ZIP_FILE} ./