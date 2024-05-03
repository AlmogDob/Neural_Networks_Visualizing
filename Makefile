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

test:
	gcc -Wall -Wextra ./test.c -lSDL2 -lSDL2_ttf -lm -o test
	./test
	rm test

