CFLAGS = -std=c11 -g -I. -I.. -O2
VPATH = ..

progs = bench_strfmt bench_printf

all: $(progs)

bench_strfmt: bench_strfmt.o strfmt.o

bench_printf: bench_printf.o

clean:
	rm *.o $(progs)
