/*
 * Copyright 2012 Andrew "Bodigrim" Lelechenko <andrew.lelechenko@gmail.com>
 *
 * This file is part of Finite Life
 *
 * Finite Life is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "life.h"

#include "cache.h"
#include "evolve.h"
#include "list.h"

void exhaustion(UINT start){
	write_cache(0, STABLE);
	UINT i;
	for(i=start; i < MAX_POS; i++){
		if(i % (1 << 20) == 0){
			printf("Now i = %llu * 1^20, %4.2f %% completed\n", (unsigned long long int)(i >> 20), (100. * i) / MAX_POS );
			}
		if(i % (1 << 20) == 0){
			write_dump_cache(i);
		}

		UINT j = i;
		uint cycle = 0;

		j = normalize(j, i);
		if(j<i)
			continue;

		do{
			push(j);
			j = evolve(j);
			if(j<i)
				break;
			j = normalize(j, i);
			cycle = elem_exists(j);
		} while(j>=i && !cycle);

		if(cycle)
			write_cache(j, 1);
		pop_all();

	}
}

int main(int argc, char* argv[]){
	cache_init();

	UINT start = 1;

	if(argc>1){
		dump_filename = argv[1];
		start = load_dump_cache();
	}
	else {
		dump_filename = "life.dump";
	}
	exhaustion(start);

	write_dump_cache(MAX_POS);

	cache_destroy();

	return 0;
}
