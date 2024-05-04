display: build_display run_display clean_display
	@echo done display

build_display: display.c
	gcc -Wall -Wextra ./display.c -lSDL2 -lSDL2_ttf -lm -o display

run_display:
	./display

clean_display:
	rm display

############################################################

dump_nn: build_dump_nn run_dump_nn clean_dump_nn
	@echo done dump_nn

build_dump_nn: dump_nn.c
	gcc -Wall -Wextra ./dump_nn.c -lm -o dump_nn

run_dump_nn:
	./dump_nn

clean_dump_nn:
	rm dump_nn

############################################################

show: build_show run_show clean_show
	@echo done show

build_show: show.c
	gcc -Wall -Wextra ./show.c -lSDL2 -lSDL2_ttf -lm -o show

run_show:
	./show

clean_show:
	rm show

############################################################

test:
	gcc -Wall -Wextra ./test.c -lSDL2 -lSDL2_ttf -lm -o test
	./test
	rm test

