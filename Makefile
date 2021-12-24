IMG=img/taric.png
EXEC=compresspng

all: $(EXEC)

release: main.c conversion.c palette.c scaling.c pngio.c
	gcc -Wall main.c conversion.c palette.c scaling.c pngio.c -lpng -lm -O2 -o $(EXEC)

debug: main.c conversion.c palette.c scaling.c pngio.c
	gcc -Wall main.c conversion.c palette.c scaling.c pngio.c -lpng -lm -g -o $(EXEC)_debug

dithering: $(EXEC) $(img)
	./$(EXEC) $(img) test_2.png -p 2
	./$(EXEC) $(img) test_2_floyd.png -p 2 -d 1
	./$(EXEC) $(img) test_2_atkinson.png -p 2 -d 2
	./$(EXEC) $(img) test_2_ordered.png -p 2 -d 3
	./$(EXEC) $(img) test_252.png -p 252
	./$(EXEC) $(img) test_252_floyd.png -p 252 -d 1
	./$(EXEC) $(img) test_252_atkinson.png -p 252 -d 2
	./$(EXEC) $(img) test_252_ordered.png -p 252 -d 3 -x

scaling: $(EXEC) $(img) 
	./$(EXEC) $(img) test_252_floyd_w100.png -p 252 -d 1 -w 100
	./$(EXEC) $(img) test_252_floyd_h100.png -p 252 -d 1 -h 100
	./$(EXEC) $(img) test_252_floyd_w1000.png -p 252 -d 1 -w 1000
	./$(EXEC) $(img) test_252_floyd_h1000.png -p 252 -d 1 -h 1000
	./$(EXEC) $(img) test_252_floyd_w2000.png -p 252 -d 1 -w 2000
	./$(EXEC) $(img) test_252_floyd_h2000.png -p 252 -d 1 -h 2000 -x

cleantest:
	rm -rf test_*.png

clean:
	rm -rf $(EXEC) $(EXEC)_debug
