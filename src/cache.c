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
#include "life.h"

static GTree* cache;

inline gint deltasign(UINT *a, UINT *b, gpointer user_data){
	return *a > *b ? 1 : (*a < *b ? -1 : 0);
}

void cache_init(){
	cache = g_tree_new_full((GCompareDataFunc)deltasign, NULL, free, free);
}

void cache_destroy(){
	g_tree_destroy(cache);
}

int cache_nnodes(){
	return g_tree_nnodes(cache);
}

UINT is_cached(UINT n){
	return g_tree_lookup(cache, &n)!=NULL;
}

uint read_cache(UINT n){
	uint* temp = g_tree_lookup(cache, &n);
	if(temp!=NULL){
		return *temp;
	}
	else{
		exit(1);
	}
}

void write_cache(UINT n, uint l){
	UINT* ptr1 = malloc(sizeof(UINT)); *ptr1 = n;
	uint* ptr2 = malloc(sizeof(uint)); *ptr2 = l;
	g_tree_replace(cache, ptr1, ptr2);
}

void rewrite_cache(UINT n, uint l){
	uint* temp = g_tree_lookup(cache, &n);
	*temp = l;
}

gboolean dump_cache_value(UINT* key, uint* value, FILE* handler){
	fprintf(handler, "%llu %i\n", (unsigned long long int)*key, *value);
	return 0;
}

void write_dump_cache(UINT i){
	FILE* handler;
	handler = fopen(dump_filename, "w");
	if(handler==NULL){
		printf("Cannot write to %s\n", dump_filename);
		exit(1);
	}

	fprintf(handler, "%llu\n", (unsigned long long int)i);
	g_tree_foreach(cache, (GTraverseFunc)dump_cache_value, handler);
	fclose(handler);
	}

UINT load_dump_cache(){
	FILE* handler;
	handler = fopen(dump_filename, "r");
	if(handler==NULL){
		// printf("Cannot read from %s\n", dump_filename);
		return 0;
	}

	UINT start;
	int ret;
	ret = fscanf(handler, "%llu\n", (unsigned long long int*)&start);
	if(ret!=1){
		printf("Cannot read start position\n");
		exit(1);
	}

	while (!feof(handler)){
		UINT key;
		uint value;
		ret =
		fscanf(handler, "%llu %i\n", (unsigned long long int*)&key, (int*)&value);
		if(ret!=2){
			printf("Cannot read key/value pair\n");
		exit(1);
		}
		write_cache(key, value);
	}
	fclose(handler);
	return start;
	}



