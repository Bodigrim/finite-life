# Copyright 2012 Andrew "Bodigrim" Lelechenko <andrew.lelechenko@gmail.com>
#
# This file is part of Finite Life
#
# Finite Life is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

N = 4
TOPOLOGY = torus
FLAGS =  -O3 -Wall -funroll-all-loops -fpeel-loops -ffast-math -fno-exceptions `pkg-config --cflags --libs glib-2.0`

life.h: life.h.template
	@echo "/* This file is generated automatically from life.h.template.\n * Do not edit it manually.\n */" > life.h
	cat life.h.template >> life.h
		@echo "#define MAX_POS ((UINT)1 << ($(N) * $(N)))" >> life.h

evolve.c: evolve.php life.h
	./evolve.php $(N) $(TOPOLOGY) > evolve.c

evolve.o: evolve.c life.h
	gcc evolve.c  -c $(FLAGS)

list.o: list.c life.h
	gcc list.c  -c $(FLAGS)

cache.o: cache.c life.h
	gcc cache.c  -c $(FLAGS)

life.o: life.c life.h cache.h evolve.h list.h
	gcc life.c  -c $(FLAGS)

undump.o: undump.c life.h cache.h evolve.h list.h
	gcc undump.c  -c $(FLAGS)

life: life.o list.o evolve.o cache.o
	gcc -o life life.o list.o evolve.o cache.o $(FLAGS)

undump: undump.o list.o evolve.o cache.o
	gcc -o undump undump.o list.o evolve.o cache.o $(FLAGS)

all: life undump

help:
	@echo "Usage: make all N=n TOPOLOGY=topology \n\
	       where n = 1..8, \n\
	       topology = torus|klein|projective\n\
	Defaults are n = 4, topology = torus\n\n\
	To remake with another parameters do not forget to run make clean first."

clean:
	rm -f life undump evolve.c life.h *.o *~
