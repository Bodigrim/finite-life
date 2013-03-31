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

/* This module implements the single encapsulated
 * list structure, where the main module store
 * an evolution sequence.
 */

/* push(n) adds its argument at the head of the list.
 * Return value is 1 if LIST_SIZE is enough
 * to store argument in list and 0 if not.
 */
int push(UINT n);

/* pop_all() simply clears the list
 * by moving pointer to the first element.
 */
void pop_all();

/* pop() pops the head of the list and returns it.
 * If list is empty it returns NULL.
 */
UINT* pop();

/* elem_pos(n) returns the first position of n
 * in the list. If n is on the top elem_pos(n) = 1.
 * If n is absent in list then return value is 0.
 */
uint elem_pos(UINT n);

/* elem_exists(n) checks whether n exists
 * in the list.
 */
uint elem_exists(UINT n);
