all: compress

compress: main.c conversion.c palette.c pngio.c
	gcc -Wall main.c conversion.c palette.c pngio.c -lpng -lm -O2 -o compress

debug: main.c conversion.c palette.c pngio.c
	gcc -Wall main.c conversion.c palette.c pngio.c -lpng -lm -g -o compress_debug

test: compress
	./compress img/taric.png taric_216.png -p 216
	./compress img/taric.png taric_216_floyd.png -p 216 -d 1
	./compress img/taric.png taric_216_atkinson.png -p 216 -d 2
	./compress img/taric.png taric_216_ordered.png -p 216 -d 3
	./compress img/taric.png taric_8.png -p 8
	./compress img/taric.png taric_8_floyd.png -p 8 -d 1
	./compress img/taric.png taric_8_atkinson.png -p 8 -d 2
	./compress img/taric.png taric_8_ordered.png -p 8 -d 3

clean:
	rm -rf compress compress_debug
