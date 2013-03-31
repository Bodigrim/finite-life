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

/* evolve(board) defines the operator of evolution of the board which takes current board state as an argument and returns the state on the next turn.
 */
extern UINT evolve(register UINT board);

/* normalize(board, edge) loops over operators of moves, apply them to board and returns the minimum. The loop is aborted if at some step move(board) < edge.
 */
extern UINT normalize(UINT board, UINT edge);