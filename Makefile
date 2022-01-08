IMG=samples/aardvark.png
EXEC=compresspng

all: release

release: main.c dithering.c palette.c scaling.c colorspace.c pngio.c
	gcc -Wall main.c dithering.c palette.c scaling.c colorspace.c pngio.c -lpng -lm -O2 -o $(EXEC)

debug: main.c dithering.c palette.c scaling.c colorspace.c pngio.c
	gcc -Wall main.c dithering.c palette.c scaling.c colorspace.c pngio.c -lpng -lm -g -o $(EXEC)_debug

samples: release $(IMG)
	./$(EXEC) $(IMG) sample_static_2.png -p 2
	./$(EXEC) $(IMG) sample_static_8.png -p 8
	./$(EXEC) $(IMG) sample_static_16.png -p 16
	./$(EXEC) $(IMG) sample_static_252.png -p 252
	./$(EXEC) $(IMG) sample_static_2_atkinson.png -p 2 -d 2
	./$(EXEC) $(IMG) sample_static_8_atkinson.png -p 8 -d 2
	./$(EXEC) $(IMG) sample_static_16_atkinson.png -p 16 -d 2
	./$(EXEC) $(IMG) sample_static_252_atkinson.png -p 252 -d 2
	./$(EXEC) $(IMG) sample_dynamic_2.png -P 2
	./$(EXEC) $(IMG) sample_dynamic_8.png -P 8
	./$(EXEC) $(IMG) sample_dynamic_16.png -P 16
	./$(EXEC) $(IMG) sample_dynamic_256.png -P 256
	./$(EXEC) $(IMG) sample_dynamic_2_atkinson.png -P 2 -d 2
	./$(EXEC) $(IMG) sample_dynamic_8_atkinson.png -P 8 -d 2
	./$(EXEC) $(IMG) sample_dynamic_16_atkinson.png -P 16 -d 2
	./$(EXEC) $(IMG) sample_dynamic_256_atkinson.png -P 256 -d 2
	./$(EXEC) $(IMG) sample_dynamic_2_fast.png -P 2
	./$(EXEC) $(IMG) sample_dynamic_8_fast.png -P 8
	./$(EXEC) $(IMG) sample_dynamic_16_fast.png -P 16
	./$(EXEC) $(IMG) sample_dynamic_256_fast.png -P 256
	./$(EXEC) $(IMG) sample_dynamic_2_fast_atkinson.png -P 2 -d 2
	./$(EXEC) $(IMG) sample_dynamic_8_fast_atkinson.png -P 8 -d 2
	./$(EXEC) $(IMG) sample_dynamic_16_fast_atkinson.png -P 16 -d 2
	./$(EXEC) $(IMG) sample_dynamic_256_fast_atkinson.png -P 256 -d 2 -x

dithering: release $(IMG)
	./$(EXEC) $(IMG) test_2.png -p 2
	./$(EXEC) $(IMG) test_2_floyd.png -p 2 -d 1
	./$(EXEC) $(IMG) test_2_atkinson.png -p 2 -d 2
	./$(EXEC) $(IMG) test_2_ordered.png -p 2 -d 3
	./$(EXEC) $(IMG) test_252.png -p 252
	./$(EXEC) $(IMG) test_252_floyd.png -p 252 -d 1
	./$(EXEC) $(IMG) test_252_atkinson.png -p 252 -d 2
	./$(EXEC) $(IMG) test_252_ordered.png -p 252 -d 3 -x

scaling: release $(IMG) 
	./$(EXEC) $(IMG) test_252_floyd_w100.png -p 252 -d 1 -w 100
	./$(EXEC) $(IMG) test_252_floyd_h100.png -p 252 -d 1 -h 100
	./$(EXEC) $(IMG) test_252_floyd_w500.png -p 252 -d 1 -w 500
	./$(EXEC) $(IMG) test_252_floyd_h500.png -p 252 -d 1 -h 500
	./$(EXEC) $(IMG) test_252_floyd_w1000.png -p 252 -d 1 -w 1000
	./$(EXEC) $(IMG) test_252_floyd_h1000.png -p 252 -d 1 -h 1000
	./$(EXEC) $(IMG) test_252_floyd_w2000.png -p 252 -d 1 -w 2000
	./$(EXEC) $(IMG) test_252_floyd_h2000.png -p 252 -d 1 -h 2000 -x

dynamic: release $(IMG)
	./$(EXEC) $(IMG) test_fast_dynamic_2.png -P 2 -f
	./$(EXEC) $(IMG) test_fast_dynamic_8.png -P 8 -f
	./$(EXEC) $(IMG) test_fast_dynamic_16.png -P 16 -f
	./$(EXEC) $(IMG) test_fast_dynamic_256.png -P 256 -f
	./$(EXEC) $(IMG) test_dynamic_2.png -P 2
	./$(EXEC) $(IMG) test_dynamic_8.png -P 8
	./$(EXEC) $(IMG) test_dynamic_16.png -P 16
	./$(EXEC) $(IMG) test_dynamic_256.png -P 256 -x


clear:
	rm -rf $(EXEC) $(EXEC)_debug
	rm -rf test_*.png
	rm -rf $(IMG)_*.png
