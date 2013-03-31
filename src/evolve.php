#!/usr/bin/php
<?php
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

/* This program is intended to generate C-code for operators of evolution and normalization for a given size and topology of the board. It's written in quick-and-dirty way because its performance is indifferent.

Size and topology are read from command line options. Normally the output is to be send to evolve.c. For example,
./evolve.php 4 torus > evolve.c

Output C-code implements following functions:
1. UINT evolve(UINT board)
	This function defines the operator of evolution of the board which takes current board state as an argument and returns the state on the next turn.
2. Auxiliary operators of moves UINT move(UINT board) such that they are commutative with evolve(). Their names are shiftNN, rotDEG, mirrorX and so on.
3. UINT normalize(UINT board, UINT edge)
	This function loops over operators of moves, apply them to board and returns the minimum. The loop is aborted if at some step move(board) < edge.
 */

/* normalize_coord(i, j) normalizes coordinates at the board with the respect to given topology. It takes pair of integers (i, j) and returns (i0, j0) where 0 <= i0, j0 < N.
 * For example, for TOPOLOGY=torus
 *   normalize_coord( 0, -1) = (  0, N-1),
 *   normalize_coord(-1, -1) = (N-1, N-1),
 *   normalize_coord(-1,  0) = (N-1, 0  );
 *
 * for TOPOLOGY=klein
 *   normalize_coord( 0, -1) = (  0, N-1),
 *   normalize_coord(-1, -1) = (N-1, 0  ),
 *   normalize_coord(-1,  0) = (N-1, N-1);
 *
 * for TOPOLOGY=projective
 *   normalize_coord( 0, -1) = (N-1, N-1),
 *   normalize_coord(-1, -1) = (  0, 0  ),
 *   normalize_coord(-1,  0) = (N-1, N-1).
 */

function normalize_coord($i, $j){
	switch(TOPOLOGY){
	case "torus":
		return array(($i + N) % N, ($j + N) % N);
	case "klein":
		if($i<0)
			return normalize_coord($i+N, N-$j-1);
		else if($i>=N)
			return normalize_coord($i-N, N-$j-1);
		else
			return array($i, ($j + N) % N);
	case "projective":
		if($i<0)
			return normalize_coord($i+N, N-$j-1);
		else if($i>=N)
			return normalize_coord($i-N, N-$j-1);
		else if($j<0)
			return normalize_coord(N-$i-1, $j+N);
		else if($j>=N)
			return normalize_coord(N-$i-1, $j-N);
		else
			return array($i, $j);
	default:
		die("Unknown topology " . TOPOLOGY);
		}
	}

/* get_cell(i, j) generates C-code to get the specified cell of the board, board[i, j]
 */
function get_cell($i, $j){
	list($i0, $j0) = normalize_coord($i, $j);
	$pos = $i0 * N + $j0;
	return "( (board >> $pos) & 1 )";
	}

/* move_cell(i1, j1, i2, j2) generates effective C-code to set cell board[i2, j2] to the cell board[i1, j1]
 */
function move_cell($i1, $j1, $i2, $j2){
	list($i1, $j1) = normalize_coord($i1, $j1);
	list($i2, $j2) = normalize_coord($i2, $j2);
	$pos1 = $i1 * N + $j1;
	$pos2 = $i2 * N + $j2;

	$ret = "(board & ((UINT)1 << $pos1))";

	if($pos1 > $pos2){
		$delta = $pos1 - $pos2;
		$ret = "( $ret >> $delta )";
		}
	else if($pos1 < $pos2){
		$delta = $pos2 - $pos1;
		$ret = "( $ret << $delta )";
		}

	return $ret;
	}

/* count_neighbours(i, j) generates C-code to count a number of the neighbours of board[i, j]
 */
function count_neighbours($i, $j){
	$ret = "( " . get_cell($i-1, $j-1) . " + " .
		get_cell($i-1, $j  ) . " + " .
		get_cell($i-1, $j+1) . " + " .
		get_cell($i,   $j-1) . " + " .
		get_cell($i,   $j+1) . " + " .
		get_cell($i+1, $j-1) . " + " .
		get_cell($i+1, $j  ) . " + " .
		get_cell($i+1, $j+1) . " )";
	return $ret;
	}

/* decide_state(i, j) generates C-code to evaluate state of the board[i, j] at the next turn
 */
function decide_state($i, $j){
	$cell = get_cell($i, $j);
	$neighbours = count_neighbours($i, $j);
	$ret = "( $neighbours==3 ? 1 : ( ($cell && $neighbours==2) ? 1 : 0 ) )";
	return $ret;
	}

/* evolve() generates C-code of the board's turn-by-turn evolution operator
 */
function evolve(){
	$ret = array();
	for($i=0; $i<N; $i++){
		for($j=0; $j<N; $j++){
			$pos = $i * N + $j;
			$state = decide_state($i, $j);
			$ret[] = "( (UINT)$state << $pos )";
			}
		}
	$ret = implode("\n\t+ ", $ret);
	$ret = "UINT evolve(register UINT board){\n\treturn $ret;\n}\n\n";
	return $ret;
	}

/* move_operator(name, fi, fj) generates C-code implementing move operator `name` which transforms board to board following the rule:
 * board[fi(i, j), fj(i, j)] -> board[i, j]
 */
function move_operator($name, $fi, $fj){
	$ret = array();
	for($i=0; $i<N; $i++){
		for($j=0; $j<N; $j++){
			$ret[] = move_cell($fi($i, $j), $fj($i, $j), $i, $j);
			}
		}
	$ret = implode("\n\t+ ", $ret);
	$ret = "inline UINT $name(UINT board){\n\treturn $ret;\n}\n\n";
	return $ret;
	}

/* shift_op(di, dj) is a move operator of shift
 */
function shift_op($di, $dj){
	$fi = function($i, $j) use($di) {return $i+$di;};
	$fj = function($i, $j) use($dj) {return $j+$dj;};
	return array($fi, $fj);
	}

/* rotate90_op() is a move operator of counter-clockwise rotation at 90 degrees
 */
function rotate90_op(){
	$fi = function($i, $j) {return N - $j - 1;};
	$fj = function($i, $j) {return $i;};
	return array($fi, $fj);
	}

/* rotate180_op() is a move operator of rotation at 180 degrees
 */
function rotate180_op(){
	$fi = function($i, $j) {return N - $i - 1;};
	$fj = function($i, $j) {return N - $j - 1;};
	return array($fi, $fj);
	}

/* rotate270_op() is a move operator of counter-clockwise rotation at 270 degrees
 */
function rotate270_op(){
	$fi = function($i, $j) {return $j;};
	$fj = function($i, $j) {return N - $i - 1;};
	return array($fi, $fj);
	}

/* mirrorH_op() is a move operator of horizontal mirroring
 */
function mirrorH_op(){
	$fi = function($i, $j) {return N - $i - 1;};
	$fj = function($i, $j) {return $j;};
	return array($fi, $fj);
	}

/* mirrorV_op() is a move operator of vertical mirroring
 */
function mirrorV_op(){
	$fi = function($i, $j) {return $i;};
	$fj = function($i, $j) {return N - $j - 1;};
	return array($fi, $fj);
	}

/* compose_ops(op1, op2) builds for given operators  op1 and op2 their composition op1(op2(...))
 */
function compose_ops($op1, $op2){
	$ret = array();
	$ret[0] = function($i, $j) use($op1, $op2) {return $op1[0]($op2[0]($i, $j), $op2[1]($i, $j));};
	$ret[1] = function($i, $j) use($op1, $op2) {return $op1[1]($op2[0]($i, $j), $op2[1]($i, $j));};
	return $ret;
	}

/* get_shifts() lists all shifts which are move operators for given topology
 */
function get_shifts(){
	$ret = array();

	switch(TOPOLOGY){
	case "torus":
		for($di=0; $di<N; $di++){
			for($dj=0; $dj<N; $dj++){
				if($di!=0 || $dj!=0)
					$ret["shift$di$dj"]= shift_op($di, $dj);
				}
			}
		break;
	case "klein":
		for($di=1; $di<N; $di++){
			$ret["shift$di"]= shift_op($di, 0);
			}
		break;
	case "projective";
		break;
	default:
		die("Unknown TOPOLOGY " . TOPOLOGY);
		break;
		}

	return $ret;
	}

/* get_rotates_and_mirrors() lists all rotates, mirrors and their compositions which are move operators for given topology
 */
function get_rotates_and_mirrors(){
	$ret = array();

	switch(TOPOLOGY){
	case "torus":
	case "projective":
		$ret["r90"]    = rotate90_op();
		$ret["r180"]   = rotate180_op();
		$ret["r270"]   = rotate270_op();
		$ret["mH"]     = mirrorH_op();
		$ret["r90mH"]  = compose_ops(rotate90_op(), mirrorH_op());
		$ret["r180mH"] = compose_ops(rotate180_op(), mirrorH_op());
		$ret["r270mH"] = compose_ops(rotate270_op(), mirrorH_op());
		break;
	case "klein":
		$ret["r180"] = rotate180_op();
		$ret["mH"]   = mirrorH_op();
		$ret["mV"]   = mirrorV_op();
		break;
	default:
		die("Unknown TOPOLOGY " . TOPOLOGY);
		break;
		}

	return $ret;
	}

/* combine_funcs(list1, list2) lists all operators from list1, list2 and their compositions
 */
function combine_funcs($list1, $list2){
	$ret = array_merge($list1, $list2);
	foreach($list1 as $name1=>$func1)
		foreach($list2 as $name2=>$func2){
			$ret["{$name1}_{$name2}"] = compose_ops($func1, $func2);
			$ret["{$name2}_{$name1}"] = compose_ops($func2, $func1);
			}
	return $ret;
	}

/* unique_funcs(funclist) loops over operators in funclist and removes duplicates (that means that they produce the same C-code)
*/
function unique_funcs($funclist){
	$ret = $codes = array();
	foreach($funclist as $funcname=>$op){
		$code = move_operator("", $op[0], $op[1]);
		if(!in_array($code, $codes)){
			$codes[] = $code;
			$ret[$funcname] = $op;
			}
		}
	return $ret;
	}

/* print_funcs(funclist) generates C-code for each operator in the funclist
*/
function print_funcs($funclist){
	$ret = array();
	foreach($funclist as $funcname=>$op){
			$ret[] = move_operator($funcname, $op[0], $op[1]);
			}
	return implode("", $ret);
	}

/* normalize(funclist) generates C-code for normalization operator with respect to a given list of move operators
 */
function normalize($funclist){
	$ret = "";
	foreach($funclist as $funcname=>$op){
		$ret.= "\ttest = $funcname(board);\n\tif(test < min){min = test; if(min < edge) return min;}\n";
		}

	$ret = "UINT normalize(UINT board, UINT edge){\n\tregister UINT test;\n\tregister UINT min = board;\n{$ret}\n\treturn min;\n}\n\n";
	return $ret;
	}

/* write_c() gathers all C-code from the funations above to produce complete evolve.c.
 */
function write_c(){
	$funclist = unique_funcs( combine_funcs( get_shifts(), get_rotates_and_mirrors()));
	//echo count($funclist); die();

	$code = "/* This file is generated automatically by life.php.\n * Do not edit it manually.\n * See documentation in evolve.h and evolve.php.\n */\n#include \"life.h\"\n\n"
	      . evolve()
	      . print_funcs($funclist)
	      . normalize($funclist);
	echo $code;
	}

/* Check command-line arguments and write error messages if needed */

if($argc < 3){
	echo "Not enough arguments! \nRun as `./life.php N topology`\nwhere N = 1..8, \n      topology = torus|klein|projective\n";
	die(1);
	}

$N = (int)$argv[1];
if($N < 1 || $N > 8){
	echo "Wrong first argument! \nN must be between 1 and 8\n";
	die(2);
	}

$topology = $argv[2];
if($topology!="torus" && $topology!="klein" && $topology!="projective"){
	echo "Wrong second argument! \ntopology must be torus (torus S^2) or klein (Klein bottle, K^2) or projective (real projective plane, RP^2)\n";
	die(3);
	}

/* Define size and topology of the board */

define("N", $N);
define("TOPOLOGY", $topology);

/* Produce output */

write_c();

?>
