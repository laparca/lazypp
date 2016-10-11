.PHONY:all clean tests
all: tests

clean:
	make -C tests clean

tests:
	make -C tests

