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

/* This program is intended to postprocess dumps of `life` program. The last write its dumps in the following format:
 * <last_checked_board>
 * <board1> <quasiperiod1>
 * <board2> <quasiperiod2>
 * ...
 *
 * The current program parse such dumps, unroll quasiperiods into periods, filters vanishing boards, duplicates of cycles and cycles which are reducing to same after normalizing. The output has format
 * <board1> <period1>
 * <board2> <period2>
 * ...
 *
 * Usage:
 * cat life.dump | ./undump > life.undump
 *
 * One can read the output directly or handle it with stat.awk to get aggregated information about cycles and their length.
 */

#include <stdio.h>
#include <stdlib.h>

#include "life.h"

#include "cache.h"
#include "evolve.h"
#include "list.h"

/* measure_period(initial) measures period for given board
 */
uint measure_period(UINT initial){
	/* Initialize variables */
	UINT board = initial;
	uint period = 0;

	do{
		write_cache(normalize(board, 0), 1);
		period++;
		board = evolve(board);
		} while(board!=initial);

	return period;
	}

/* explain_dump() reads dump from standard input lin-by-line and writes to standard output
 */
UINT explain_dump(){
	int ret;

	/* First we read last_checked_board.
	 * In fact we don't make any use of it - just read it for compatibility with dump format
	 */
	UINT last;
	ret = scanf("%llu\n", (unsigned long long int*)&last);
	if(ret!=1){
		printf("Cannot read last_checked_board");
		exit(1);
	}

	/* Read dump line-by-line */
	while (1){
		UINT board;
		uint period;

		/* Read board and quasiperiod */
		ret = scanf("%llu %i\n", (unsigned long long int*)&board, (int*)&period);
		/* If the end of file is reached then abort */
		if(ret==EOF){
			return 0;
			}
		/* If not exactly two integers are read then abort */
		if(ret!=2){
			printf("Cannot read key/value pair");
			exit(1);
		}
		/* If board is empty or vanishing or duplicate then abort */
		if(!board || !period || is_cached(normalize(board, 0)))
			continue;

		period = measure_period(board);
		if(!period)
			continue;

		/* Write output */
		printf("%llu %i\n", (unsigned long long int)board, (int)period);
	}
}

int main(){
	cache_init();
	explain_dump();
	return 0;
}