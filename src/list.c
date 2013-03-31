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

#include "life.h"

/* This module implements the single encapsulated
 * list structure, where the main module store
 * an evolution sequence.
 *
 * For the sake of speed and simplicity we implement
 * it as a fixed-sized array list[LIST_SIZE]
 * with a pointer to the head element.
 */

/* It seems that for our task
 * the following maximum list size is enough.
 * Increase if needed.
 */
#define LIST_SIZE 10240

/* list is a storage for the list,
 * list_counter is a number of elements in list,
 * so list[list_counter-1] points
 * to the head of the list
 *
 * We mark them as static to prevent unwrapped access
 * from other modules.
 */
static UINT list[LIST_SIZE];
static uint list_counter = 0;

/* push(n) adds its argument at the head of the list.
 * Return value is 1 if LIST_SIZE is enough
 * to store argument in list and 0 if not.
 */
int push(UINT n){
	if( list_counter >= LIST_SIZE ){
		return 0;
	}
	list[list_counter] = n;
	list_counter++;
	return 1;
}

/* pop_all() simply clears the list
 * by moving pointer to the first element.
 */
void pop_all(){
	list_counter = 0;
	}

/* pop() pops the head of the list and returns it.
 * If list is empty it returns NULL.
 */
UINT* pop(){
	if(list_counter==0)
		return NULL;
	list_counter--;
	return &list[list_counter];
}

/* elem_pos(n) returns the first position of n
 * in the list. If n is on the top then elem_pos(n) = 1.
 * If n is absent in list then return value is 0.
 */
uint elem_pos(UINT n){
	uint pos;
	for(pos = list_counter-1; pos >= 0; pos--){
		if(list[pos]==n)
			return list_counter - pos;
		}
	return 0;
}

/* elem_exists(n) checks whether n exists
 * in the list.
 */
uint elem_exists(UINT n){
	uint pos;
	for(pos = list_counter-1; pos>=0; pos--){
		if(list[pos]==n)
			return 1;
		}
	return 0;
}

