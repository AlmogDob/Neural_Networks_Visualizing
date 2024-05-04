display: build_display run_display clean_display
	@echo done display

build_display: display.c
	gcc -Wall -Wextra ./display.c -lSDL2 -lSDL2_ttf -lm -o display

run_display:
	./display

clean_display:
	rm display

############################################################

nn: build_nn run_nn clean_nn
	@echo done nn

build_nn: nn.c
	gcc -Wall -Wextra ./nn.c -lSDL2 -lSDL2_ttf -lm -o nn

run_nn:
	./nn

clean_nn:
	rm nn

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

