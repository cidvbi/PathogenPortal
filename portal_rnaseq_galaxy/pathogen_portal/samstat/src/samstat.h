/*
 
 Copyright (C) 2010 Timo Lassmann <timolassmann@gmail.com>
 
 This file is part of SAMstat.
 
 Delve is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Delve is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Delve.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_SEQ_LEN 5000
#define MAX_LINE 20000
#define LIST_STORE_SIZE 1

#define MAXERROR 100
#define VERSION 1.08
#define KMERLEN 10
#define KMERALLOC (1 << ( KMERLEN * 2))




struct mer_info{
	int count;
	float p_value;
	int identifier;
};

struct seq_stats{
	int** seq_len;
	int*** kmers;
	int*** nuc_composition;
	int** seq_quality;
	int** posterior_mappability;
	int* aln_quality;
	int* alignments;
	struct mer_info*** ten_mers;
	int* nuc_num;
	float** errors;
	double* percent_identity;
	int*** mismatches;
	int*** insertions;
	int** deletions;
	int sam;
	int md;
	int min_len;
	int max_len;
};


struct seq_stats* init_seq_stats(int kmer_len);
void free_seq_stats(struct seq_stats* seq_stats);
void print_summary(struct seq_stats* seq_stats,struct parameters* param,int file_num,FILE* outfile);
struct seq_stats* collect_data(struct seq_stats* seq_stats,struct parameters* param,int (*fp)(struct read_info** ,struct parameters*,FILE* ),int file_num);
struct seq_stats* get_top_x_mers(struct seq_stats* seq_stats,struct parameters* param);
void print_seq_stats(struct seq_stats* seq_stats,struct parameters* param);
int sanity_check(struct seq_stats* seq_stats);

int parse_cigar_md(struct read_info* ri,struct seq_stats* seq_stats,int qual_key);

