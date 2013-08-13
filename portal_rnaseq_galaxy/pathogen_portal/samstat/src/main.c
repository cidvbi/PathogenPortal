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


#include "interface.h"
#include "nuc_code.h"
#include "io.h"
#include "misc.h"
#include "samstat.h" 
#include <math.h>
#include "html_out.h"

int main (int argc,char * argv[]) {
	struct parameters* param = 0;
	struct seq_stats* seq_stats = 0;
	FILE* outfile =0;
	int i;
	
	init_nuc_code();
	
	param = interface(param,argc,argv);
	if(param->summary){
		if ((outfile = fopen(param->summary, "w")) == NULL){
			fprintf(stderr,"can't open output\n");
			exit(-1);
		}
	}
	
	if(param->format || (!param->infiles && !isatty(0))){
		if(!param->format){
			fprintf(stderr,"No format specified. Use -f <sam | bam | fa | fq > \n");
			exit(-1);
		}
		if(!strcmp("sam", param->format)){
			param->sam = 1;
			//}else if (byg_end(".bam", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("bam",  param->format)){
			param->sam = 2;
			//}else if (byg_end(".fa", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("fa", param->format)){
			param->sam = 0;
			//}else if (byg_end(".fq", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("fq",  param->format)){
			param->sam = 0;
			//}else if (byg_end(".fastq", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("fastq",  param->format)){
			param->sam = 0;
			//}else if (byg_end(".fastaq", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("fastaq",  param->format)){
			param->sam = 0;
			//}else if (byg_end(".fasta", param->infile[i])  == strlen(param->infile[i])){
		}else if (!strcmp("fasta",  param->format)){
			param->sam = 0;
		}else{
			param->sam = -1;
		}
		if(param->sam != -1 && !param->infiles && !isatty(0)){
			fprintf(stdout,"Working on: stdin\n");
			seq_stats = init_seq_stats(param->kmer_size);
			seq_stats->sam = param->sam;
			if(param->sam == 0){
				seq_stats = collect_data(seq_stats,param,&read_fasta_fastq,-1);
			}else if(param->sam == 2){
				seq_stats = collect_data(seq_stats,param,&read_sam_chunk,-1);
			}else{
				seq_stats = collect_data(seq_stats,param,&read_sam_chunk,-1);
			}
			
			if(sanity_check(seq_stats)){
				if(param->summary){
					print_summary(seq_stats,param,-1,outfile);
				}else{
					print_html_page(seq_stats,param,-1);
				}
			}
			free_seq_stats(seq_stats);
		}
	}
	
	for(i = 0; i < param->infiles;i++){
		if(!param->format || param->sam == -1){
			param->sam = 0;
			
			//if(byg_end(".sam", param->infile[i])   == strlen(param->infile[i])){
			if(!strcmp(".sam", param->infile[i] + (strlen(param->infile[i] ) - 4))){
				param->sam = 1;
			//}else if (byg_end(".bam", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".bam", param->infile[i] + (strlen(param->infile[i] ) - 4))){
				param->sam = 2;
			//}else if (byg_end(".fa", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fa", param->infile[i] + (strlen(param->infile[i] ) - 3))){
				param->sam = 0;
			//}else if (byg_end(".fq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fq", param->infile[i] + (strlen(param->infile[i] ) - 3))){
				param->sam = 0;
			//}else if (byg_end(".fastq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fastq", param->infile[i] + (strlen(param->infile[i] ) - 6))){
				param->sam = 0;
			//}else if (byg_end(".fastaq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fastaq", param->infile[i] + (strlen(param->infile[i] ) - 7))){
				param->sam = 0;
			//}else if (byg_end(".fasta", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fasta", param->infile[i] + (strlen(param->infile[i] ) - 6))){
				param->sam = 0;
			}else if(!strcmp(".sam.gz", param->infile[i] + (strlen(param->infile[i] ) - 7))){
				param->sam = 1;
				param->gzipped  = 1;
				//}else if (byg_end(".bam", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".bam.gz", param->infile[i] + (strlen(param->infile[i] ) - 7))){
				param->sam = 2;
				param->gzipped  = 1;
				//}else if (byg_end(".fa", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fa.gz", param->infile[i] + (strlen(param->infile[i] ) - 6))){
				param->sam = 0;
				param->gzipped  = 1;
				//}else if (byg_end(".fq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fq.gz", param->infile[i] + (strlen(param->infile[i] ) - 6))){
				param->sam = 0;
				param->gzipped  = 1;
				//}else if (byg_end(".fastq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fastq.gz", param->infile[i] + (strlen(param->infile[i] ) - 9))){
				param->sam = 0;
				param->gzipped  = 1;
				//}else if (byg_end(".fastaq", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fastaq.gz", param->infile[i] + (strlen(param->infile[i] ) - 10))){
				param->sam = 0;
				param->gzipped  = 1;
				//}else if (byg_end(".fasta", param->infile[i])  == strlen(param->infile[i])){
			}else if (!strcmp(".fasta.gz", param->infile[i] + (strlen(param->infile[i] ) - 9))){
				param->sam = 0;
				param->gzipped  = 1;
			}else{
				param->sam = -1;
			}
		}
		//fprintf(stdout,"Loking at on:%s	%d\n",param->infile[i],sam);
		if(param->sam != -1){
			fprintf(stdout,"Working on:%s\n",param->infile[i]);
			seq_stats = init_seq_stats(param->kmer_size);
			seq_stats->sam = param->sam;
			if(param->sam == 0){
				seq_stats = collect_data(seq_stats,param,&read_fasta_fastq,i);
			}else if(param->sam == 2){
				seq_stats = collect_data(seq_stats,param,&read_sam_chunk,i);
			}else{
				seq_stats = collect_data(seq_stats,param,&read_sam_chunk,i);
			}
			
			if(sanity_check(seq_stats)){
				if(param->summary){
					print_summary(seq_stats,param,i,outfile);
				}else{
					print_html_page(seq_stats,param,i);
				}
			}
			free_seq_stats(seq_stats);
		}		
	}
	if(param->summary){
		fclose(outfile);
	}
	free_param(param);
	return 0;
}

void print_summary(struct seq_stats* seq_stats,struct parameters* param,int file_num,FILE* outfile)
{
	int i,j,c;
	char buffer [80];
	float total;
	float average_length;
	float di_nuc_counter;
	float total_length;
	char alphabet[] = "ACGTN";
	static int gaga = 1;
	struct tm* clock = 0;// create a time structure
	struct stat attrib;// create a file attribute structure
	
	if(file_num != -1){
		stat(param->infile[file_num], &attrib);// get the attributes of afile.txt
	
		clock = gmtime(&(attrib.st_mtime));
	}
	if(gaga){
		fprintf(outfile,"File name\t");
		fprintf(outfile,"Year\t");
		fprintf(outfile,"Month\t");
		fprintf(outfile,"Day\t");
		//fprintf(outfile,"#Sequences\t");
		fprintf(outfile,"avgLen\t");
		fprintf(outfile,"avgError Q>21\t");
		fprintf(outfile,"avgError Q>11\t");
		fprintf(outfile,"avgError Q>3\t");
		fprintf(outfile,"avgError\t");
		fprintf(outfile,"%%ID Q>21\t");
		fprintf(outfile,"%%ID Q>11\t");
		fprintf(outfile,"%%ID Q>3\t");
		fprintf(outfile,"%%ID\t");
		fprintf(outfile,"#Q>21\t");
		fprintf(outfile,"#Q>11\t");
		fprintf(outfile,"#Q>3\t");
		fprintf(outfile,"#All \t");
		
		for(i = 0; i < 4;i++){
			for(j = 0; j < 4;j++){
				fprintf(outfile,"%c%c\t",alphabet[i],alphabet[j] );
			}
		}
		fprintf(outfile,"\n");
		gaga = 0;
	}
	if(param->label){
		fprintf(outfile,"%s\t",param->label);
	}
	else if(param->alt_lib_name){
		fprintf(outfile,"%s\t",param->alt_lib_name);
	}else if(file_num == -1){
		fprintf(outfile,"%s\t","stdin");
	}else{
		fprintf(outfile,"%s\t",shorten_pathname(param->infile[file_num])) ;
	}
	
	if(file_num== -1){
		sprintf(buffer,"NA\tNA\tNA");
		fprintf(outfile,"%s\t",buffer);
	}else {
		strftime (buffer,80,"%Y\t%b\t%d",clock);
		fprintf(outfile,"%s\t",buffer);
	}
	
	total = 0.0;
	for(i= 0;i < 6;i++){
		total += seq_stats->alignments[i];
	}
	//fprintf(outfile,"%d\t",(int)total );
	total_length = 0.0;
	average_length = 0.0;
	for(i = 0; i < 6;i++){
		for(j = 0; j < MAX_SEQ_LEN;j++){
			
			average_length +=  (float)(j* seq_stats->seq_len[i][j]);
			total_length += (float)(j* seq_stats->seq_len[i][j]);
		}
	}
	average_length= average_length / total;
	fprintf(outfile,"%0.2f\t",average_length );
	
	//error
	average_length = 0.0;
	total_length = 0.0;
	for(j =0;j < MAXERROR;j++){
		average_length += (float) (j*seq_stats->errors[4][j]);
		average_length += (float) (j*seq_stats->errors[3][j]);
	}
	for(j =0;j < MAX_SEQ_LEN;j++){
		total_length += (float)(j* seq_stats->seq_len[4][j]);
		total_length += (float)(j* seq_stats->seq_len[3][j]);
	}
	fprintf(outfile,"%0.2f\t",average_length / total_length *100.0f);
	
	for(j =0;j < MAXERROR;j++){
		average_length += (float) (j*seq_stats->errors[2][j]);
	}
	for(j =0;j < MAX_SEQ_LEN;j++){
		total_length += (float)(j* seq_stats->seq_len[2][j]);
	}
	fprintf(outfile,"%0.2f\t",average_length / total_length*100.0f);
	
	for(j =0;j < MAXERROR;j++){
		average_length += (float) (j*seq_stats->errors[1][j]);
	}
	for(j =0;j < MAX_SEQ_LEN;j++){
		total_length += (float)(j* seq_stats->seq_len[1][j]);
	}
	fprintf(outfile,"%0.2f\t",average_length / total_length*100.0f);
	
	for(j =0;j < MAXERROR;j++){
		average_length += (float) (j*seq_stats->errors[0][j]);
	}
	for(j =0;j < MAX_SEQ_LEN;j++){
		total_length += (float)(j* seq_stats->seq_len[0][j]);
	}
	fprintf(outfile,"%0.2f\t",average_length / total_length*100.0f);

	//percentage_identity;
	average_length = 0.0;
	total_length = 0.0;
	//r(j =0;j < 1001;j++){
		average_length += seq_stats->percent_identity[4];
		average_length += seq_stats->percent_identity[3];
	//}
	
	total_length +=  seq_stats->alignments[4];
	total_length +=  seq_stats->alignments[3];
	fprintf(outfile,"%0.2f\t",average_length / total_length);
	//fprintf(stderr,"Q4,3: %f	%f	\n",average_length,total_length);
	
	average_length += seq_stats->percent_identity[2];
	total_length +=  seq_stats->alignments[2];
	fprintf(outfile,"%0.2f\t",average_length / total_length);
	
	average_length += seq_stats->percent_identity[1];
	total_length +=  seq_stats->alignments[1];
	fprintf(outfile,"%0.2f\t",average_length / total_length);
	
	average_length += seq_stats->percent_identity[0];
	total_length +=  seq_stats->alignments[0];
	fprintf(outfile,"%0.2f\t",average_length / total_length);
	
	total_length = 0.0;
	total_length +=  seq_stats->alignments[4];
	total_length +=  seq_stats->alignments[3];
	fprintf(outfile,"%d\t",(int)total_length);
	total_length +=  seq_stats->alignments[2];
	fprintf(outfile,"%d\t",(int)total_length);
	total_length +=  seq_stats->alignments[1];
	fprintf(outfile,"%d\t",(int)total_length);
	total_length +=  seq_stats->alignments[0];
	fprintf(outfile,"%d\t",(int)total_length);
		
	average_length = 0;
	for(i = 0; i < 6;i++){
		for(c = 0; c < seq_stats->max_len;c++){
			for(j = 0; j < 16;j++){
				average_length += seq_stats->kmers[i][c][j]; 
			}
		}
	}
	for(j = 0; j < 16;j++){
		di_nuc_counter = 0;
		for(i = 0; i < 6;i++){
			for(c = 0; c < seq_stats->max_len;c++){
				di_nuc_counter += seq_stats->kmers[i][c][j]; 
			}
		}
		fprintf(outfile,"%0.1f\t",di_nuc_counter / average_length * 100.0f);
	}
	fprintf(outfile,"\n");
	fflush(outfile);
}

int sanity_check(struct seq_stats* seq_stats)
{
	int i;
	int sum = 0;
	for (i = 0; i < 6;i++){
		sum += seq_stats->alignments[i];
	}
	if(!sum){
		return 0;
	}
	return 1;
}


struct seq_stats* collect_data(struct seq_stats* seq_stats,struct parameters* param,int (*fp)(struct read_info** ,struct parameters*,FILE* ),int file_num)
{
	struct read_info** ri = 0;
	int i,j,c;
	int test = 1;
	int numseq;
	int qual_key = 0;
	int aln_len = 0;
	//char command[1000];
	//char  tmp[1000];
	FILE* file = 0;
	FILE* unmapped = 0;
	
	ri = malloc(sizeof(struct read_info*) * param->num_query);
	
	for(i = 0; i < param->num_query;i++){
		ri[i] = malloc(sizeof(struct read_info));
		ri[i]->seq = 0;
		ri[i]->name = 0;
		ri[i]->qual = 0;
		ri[i]->len = 0;
		ri[i]->cigar = 0;
		ri[i]->md = 0;
		ri[i]->xp = 0;
		ri[i]->priors = 0;// malloc(sizeof(unsigned int)* (LIST_STORE_SIZE+1));
		ri[i]->strand = malloc(sizeof(unsigned int)* (LIST_STORE_SIZE+1));
		ri[i]->hits = malloc(sizeof(unsigned int)* (LIST_STORE_SIZE+1));
		ri[i]->identity = malloc(sizeof(float)* (LIST_STORE_SIZE+1));
	}
	file =  io_handler(file, file_num,param);
	
	if(param->print_unmapped){
		if (!(unmapped = fopen(param->print_unmapped, "w" ))){
			fprintf(stderr,"Cannot open file '%s'\n",param->print_unmapped);
			exit(-1);
		}
	}
	
	while ((numseq = fp(ri, param,file)) != 0){
		//fprintf(stderr,"rread: %d\n",numseq);
		for(i = 0; i < numseq;i++){
			if(ri[i]->len > seq_stats->max_len){
				seq_stats->max_len = ri[i]->len;
			}
			
			if(ri[i]->len < seq_stats->min_len ){
				seq_stats->min_len = ri[i]->len;
			}
			if(ri[i]->hits[0] == 0){
				qual_key = 5;
				if(param->print_unmapped){
					print_seq(ri[i],unmapped);
				}				
			}else{
				if(ri[i]->mapq < 3){
					qual_key = 0;
				}else if(ri[i]->mapq < 10){
					qual_key = 1;
				}else if(ri[i]->mapq < 20){
					qual_key = 2;
				}else if(ri[i]->mapq < 30){
					qual_key = 3;
				}else{
					qual_key = 4;
				}
			}
			
			if(ri[i]->errors > param->k_errors_allowed && param->k_errors_allowed != -1){
				qual_key = 5;
			}
			
			if(ri[i]->cigar && ri[i]->md){
				if(ri[i]->cigar[0] != '*'){
					aln_len = parse_cigar_md(ri[i],seq_stats, qual_key);
					seq_stats->md = 1;
				}
			}
			
			if(ri[i]->strand[0] != 0){
				ri[i]->seq = reverse_complement2(ri[i]->seq,ri[i]->len);
				if(ri[i]->qual[0] != '*'){
					ri[i]->qual = reverse_without_complement(ri[i]->qual, ri[i]->len);
				}
				
				if(ri[i]->xp){
					ri[i]->xp =  reverse_without_complement(ri[i]->xp, ri[i]->len);
				}
				
			}
			if(ri[i]->qual){
				if(ri[i]->qual[0] != '*'){
					param->print_qual  = 1;
					for(j = 0; j < ri[i]->len;j++){
						
						seq_stats->seq_quality[qual_key][j] += (int)(ri[i]->qual[j]);
						
						
						if((int)(ri[i]->qual[j]) > 80){
							param->solexa = 1;
						}
						
						//fprintf(stderr,"%d	%d	%c	%d\n",j,qual_key,ri[i]->qual[j] , (int)(ri[i]->qual[j] -33));
					}
				}
			}
			
			if(ri[i]->xp){
				
				param->print_posteriors  = 1;
				for(j = 0; j < ri[i]->len;j++){
					seq_stats->posterior_mappability[qual_key][j] += (int)(ri[i]->xp[j])-33;
				}
				
			}
			
			seq_stats->alignments[qual_key]++;
			
			// sequence length
			if(ri[i]->len >=  MAX_SEQ_LEN){
				seq_stats->seq_len[qual_key][MAX_SEQ_LEN-1]++;
			}else{
				seq_stats->seq_len[qual_key][ri[i]->len]++;
			}
			// sequence composition
			for(j = 0;j <  ri[i]->len;j++){
				seq_stats->nuc_num[ri[i]->seq[j]]++; 
				seq_stats->nuc_composition[qual_key][j][ri[i]->seq[j]]++; 
			}
			
			//kmers 
			for(j = 0;j <=  ri[i]->len - param->kmer_size;j++){
				//check for 'N'
				test = 1;
				for(c = j;c < j + param->kmer_size;c++){
					if(ri[i]->seq[c] >= 4){
						test = 0;
						break;
					}
				}
				// no 'N'?  ok proceed
				
				if(test){
					test = 0;
					for(c = j;c < j + param->kmer_size;c++){
						test = test << 2 | ri[i]->seq[c];
					}
					
					seq_stats->kmers[qual_key][j][test]++;    
				}
			}
			
			// ten mers.
			for(j = 0;j <=  ri[i]->len - KMERLEN;j++){
				//check for 'N'
				test = 1;
				for(c = j;c < j + KMERLEN;c++){
					if(ri[i]->seq[c] >= 4){
						test = 0;
						break;
					}
				}
				// no 'N'?  ok proceed
				
				if(test){
					test = 0;
					for(c = j;c < j + KMERLEN;c++){
						test = test << 2 | ri[i]->seq[c];
					}
					seq_stats->ten_mers[qual_key][test]->count++;    
					//seq_stats->overall_kmers[test]+= 1.0f;
				}
			}
			
			//errors
			
			if(ri[i]->errors != -1){
				
				
				seq_stats->percent_identity[qual_key] +=(((double)aln_len - (double)ri[i]->errors) / (double)aln_len * 100.0);// ((float)aln_len - (float)ri[i]->errors) / (float) aln_len * 100.0f;
				
				//if((int)  floor((((float)aln_len - (float)ri[i]->errors) / (float)aln_len * 1000.0f) + 0.5f ) > 1000 || (int)  floor((((float)aln_len - (float)ri[i]->errors) / (float)aln_len * 1000.0f) + 0.5f ) < 0 ){
				//	fprintf(stderr,"ERROR: %f	%f\n", (float)aln_len,  (float)ri[i]->errors);
				//}	
				
				//fprintf(stderr,"%d	%d	%d	%d\n",qual_key,aln_len,ri[i]->errors,(int)  floor((((float)aln_len - (float)ri[i]->errors) / (float)aln_len * 1000.0f) + 0.5 ));
				if(ri[i]->errors >= MAXERROR){
					seq_stats->errors[qual_key][MAXERROR-1]++;
				}else{
					seq_stats->errors[qual_key][ri[i]->errors]++;
				}
			}
			
			
		}
	}
	
	if(param->print_unmapped){
		fclose(unmapped);
	}
		
	for(i = 0; i < param->num_query;i++){
		free(ri[i]->strand); 
		free(ri[i]->hits);
		free(ri[i]->identity);
		
		if(ri[i]->cigar){
			free(ri[i]->cigar);
		}
		if(ri[i]->md){
			free(ri[i]->md);
		}
		
		free(ri[i]);
	}
	free(ri);
	//fprintf(stderr,"%p\n",file);
	if(param->sam == 2 || param->sam == 1){
		pclose(file);
	}else{
		//if(file_num != -1){
		fclose(file);
		//}
	}
	return seq_stats;
}

struct seq_stats* init_seq_stats(int kmer_len)
{
	struct seq_stats* seq_stats = 0;
	int i,j,c;
	seq_stats = malloc(sizeof(struct seq_stats));
	seq_stats->alignments = malloc(sizeof(int)* 6);
	seq_stats->seq_len = malloc(sizeof(int*)* 6);
	seq_stats->kmers = malloc(sizeof(int**)* 6);
	seq_stats->nuc_composition = malloc(sizeof(int**)* 6);
	seq_stats->seq_quality = malloc(sizeof(int*)* 6);
	seq_stats->posterior_mappability = malloc(sizeof(int*)* 6);
	seq_stats->aln_quality = malloc(sizeof(int)*6);
	seq_stats->ten_mers = malloc(sizeof(struct mer_info**)*6);// KMERALLOC);
	seq_stats->nuc_num = malloc(sizeof(int) * 6);
	//seq_stats->overall_kmers= malloc(sizeof(float) * KMERALLOC);
	seq_stats->errors = malloc(sizeof(float*)* 6 );
	
	seq_stats->percent_identity = malloc(sizeof(double)* 6 );
	
	seq_stats->mismatches = malloc(sizeof(int**)* 6);
	seq_stats->insertions = malloc(sizeof(int**) * 6);
	seq_stats->deletions = malloc(sizeof(int*) * 6);
	
	for(c = 0; c < 6;c++){
		seq_stats->mismatches[c] = malloc(sizeof(int*)* MAX_SEQ_LEN);
		seq_stats->insertions[c] = malloc(sizeof(int*) * MAX_SEQ_LEN);
		seq_stats->deletions[c] = malloc(sizeof(int) * MAX_SEQ_LEN);
	
	
		for(i= 0; i < MAX_SEQ_LEN;i++){
			seq_stats->deletions[c][i] = 0;
			seq_stats->mismatches[c][i] = malloc(sizeof(int)*5);
			seq_stats->insertions[c][i] = malloc(sizeof(int)*5);
			for(j = 0; j < 5;j++){
				seq_stats->mismatches[c][i][j] = 0;
				seq_stats->insertions[c][i][j] = 0;

			}
		}
	}
	
	seq_stats->sam = 0;
	seq_stats->md = 0;
	seq_stats->min_len = 1000000000;
	seq_stats->max_len = -1000000000;
	
	for(i = 0; i < 6;i++){
		seq_stats->errors[i] = malloc(sizeof(float)* MAXERROR );
		seq_stats->percent_identity[i] = 0.0f;
		for(c = 0; c < MAXERROR;c++){
			seq_stats->errors[i][c] = 0;
			
		}
		
	}
	
	for(i = 0; i < 6;i++){
		seq_stats->nuc_num[i] = 0;
		seq_stats->alignments[i] = 0;
		seq_stats->ten_mers[i] = malloc(sizeof(struct mer_info**)* KMERALLOC);

		seq_stats->seq_len[i] = malloc(sizeof(int)* MAX_SEQ_LEN);
		seq_stats->kmers[i] = malloc(sizeof(int*)* MAX_SEQ_LEN);
		seq_stats->nuc_composition[i] = malloc(sizeof(int*)* MAX_SEQ_LEN);
		seq_stats->seq_quality[i] = malloc(sizeof(int)* MAX_SEQ_LEN);
		seq_stats->posterior_mappability[i] = malloc(sizeof(int)* MAX_SEQ_LEN);
		for(j = 0; j < KMERALLOC;j++){
			seq_stats->ten_mers[i][j] = malloc(sizeof (struct  mer_info));
			seq_stats->ten_mers[i][j]->count = 0;
			seq_stats->ten_mers[i][j]->identifier = j;
		}
		for(j = 0; j < MAX_SEQ_LEN;j++){
			seq_stats->seq_len[i][j] = 0;
			seq_stats->kmers[i][j] = malloc(sizeof(int) * (1 <<(kmer_len *2)));
			for(c = 0; c < (1 <<(kmer_len *2));c++){
				seq_stats->kmers[i][j][c] = 0;
			}
			seq_stats->nuc_composition[i][j] = malloc(sizeof(int*)* 5);
			for(c = 0; c < 5;c++){
				seq_stats->nuc_composition[i][j][c] = 0;
			}
			seq_stats->seq_quality[i][j] = 0;
			seq_stats->posterior_mappability[i][j] = 0;
		}
		
	}
	return seq_stats;
}

void free_seq_stats(struct seq_stats* seq_stats)
{
	int i,j;

	for(j = 0; j < 6;j++){
		for(i= 0; i < MAX_SEQ_LEN;i++){
			free(seq_stats->mismatches[j][i]);// = malloc(sizeof(int)*5);
			free(seq_stats->insertions[j][i]);// = malloc(sizeof(int)*5);
		}
		free(seq_stats->mismatches[j]);// = malloc(sizeof(int*)* MAX_SEQ_LEN);
		free(seq_stats->insertions[j]);// = malloc(sizeof(int*) * MAX_SEQ_LEN);
		free(seq_stats->deletions[j]);// = malloc(sizeof(int) * MAX_SEQ_LEN);
	}
	free(seq_stats->mismatches);// = malloc(sizeof(int*)* MAX_SEQ_LEN);
	free(seq_stats->insertions);// = malloc(sizeof(int*) * MAX_SEQ_LEN);
	free(seq_stats->deletions);// = malloc(sizeof(int) * MAX_SEQ_LEN);
	
	for(i = 0; i < 6;i++){

		for(j = 0; j < MAX_SEQ_LEN;j++){
			free(seq_stats->kmers[i][j]);// = malloc(sizeof(int) * (1 <<(kmer_len *2)));
			free(seq_stats->nuc_composition[i][j]);// = malloc(sizeof(int*)* 5);
			//free(seq_stats->seq_quality[i][j]);// = malloc(sizeof(int)* 6);

		}
		//free(seq_stats->seq_quality[i]);
		free(seq_stats->seq_len[i]);// = malloc(sizeof(int)* MAX_SEQ_LEN);
		free(seq_stats->kmers[i]);// = malloc(sizeof(int*)* MAX_SEQ_LEN);
		free(seq_stats->nuc_composition[i]);// = malloc(sizeof(int*)* MAX_SEQ_LEN);
		free(seq_stats->seq_quality[i]);// = malloc(sizeof(int*)* MAX_SEQ_LEN);
		for(j = 0; j < KMERALLOC;j++){
			
			
			free(seq_stats->ten_mers[i][j]); 
		}
		free(seq_stats->ten_mers[i]);
		free(seq_stats->posterior_mappability[i]);
	}
	for(i = 0; i < 6;i++){
		free(seq_stats->errors[i]);
		//free(seq_stats->percent_identity[i]);
		
	}
	free(seq_stats->posterior_mappability);
	free(seq_stats->errors);
	free(seq_stats->percent_identity);
	//free(seq_stats->overall_kmers);
	free(seq_stats->alignments);
	free(seq_stats->nuc_num);
	free(seq_stats->ten_mers);
	free(seq_stats->seq_len);// = malloc(sizeof(int*)* 6);
	free(seq_stats->kmers);// = malloc(sizeof(int**)* 6);
	free(seq_stats->nuc_composition);// = malloc(sizeof(int**)* 6);
	free(seq_stats->seq_quality);// = malloc(sizeof(int**)* 6);
	free(seq_stats->aln_quality);// = malloc(sizeof(int)*6);
	free(seq_stats);// = malloc(sizeof(struct seq_stats));
	
}


int parse_cigar_md(struct read_info* ri,struct seq_stats* seq_stats,int qual_key)
{
	int* read = malloc(sizeof(int)* MAX_SEQ_LEN);
	int* genome = malloc(sizeof(int) * MAX_SEQ_LEN);
	int reverse_int[5]  ={3,2,1,0,4};
	char tmp_num[8];
	int l,i,j,c,rp,gp,sp,exit_loop,aln_len,add; 
	
	for(i = 0; i < MAX_SEQ_LEN;i++){
		genome[i] = 0;
		read[i] = 0;
	}
	
	l = strlen((char*)ri->cigar);
	exit_loop = 0;
	i =0;
	rp = 0;
	sp = 0;
	while(!exit_loop){
		c = 0;
		if(isdigit((int)ri->cigar[i])){
			j = 0;
			while (isdigit(ri->cigar[i])) {
				tmp_num[j] = ri->cigar[i];
				j++;
				i++;
				if(i == l){
					exit_loop =1;
					break;
				}
			}
			tmp_num[j] = 0;
			
			c = atoi(tmp_num);
		}
		if(isalpha((int)ri->cigar[i])){
			switch (ri->cigar[i]) {
				case 'M':
					for(j = 0; j < c;j++){
						read[rp] = ri->seq[sp];
						rp++;
						sp++;
					}
		//			fprintf(stderr,"M:%d\n",c);
					break;
				case 'I':
					for(j = 0; j < c;j++){
						read[rp] = ri->seq[sp];
						genome[rp] = -1;
						rp++;
						sp++;
					}
					
		//			fprintf(stderr,"I:%d\n",c);
					break;
				case 'D':
					for(j = 0; j < c;j++){
						read[rp] = -1;
						rp++;
						
					}
					
		//			fprintf(stderr,"D:%d\n",c);
					break;
				default:
					break;
			}
			c = 0;
		}
		i++;
		if(i == l){
			exit_loop =1;
			break;
		}
		
	}
	aln_len = rp;
	
	i =0;
	rp = 0;
	
	while(read[rp] == -1){
		rp++;
	}
	gp = 0;
	exit_loop = 0;
	add  = 0;
	l = strlen((char*)ri->md);
	
	//int gg;
	
//#1C20^ATT0C3A0
	while(!exit_loop){
		if(isdigit((int)ri->md[i])){
			j = 0;
			while (isdigit(ri->md[i])) {
				tmp_num[j] = ri->md[i];
				j++;
				i++;
				if(i == l){
					exit_loop = 1;
					break;
				}
			}
			tmp_num[j] = 0;
			
			c = atoi(tmp_num);
			
			//fprintf(stderr,"MD:%d\n",c);
			for(j = 0; j < c;j++){
				while(genome[gp] == -1){
					gp++;
					rp++;
				}
				while(read[rp] == -1){
					rp++;
				}
				genome[gp] = read[rp];
				
				gp++;
				rp++;
				//fprintf(stderr,"%d	%d	%d \n",aln_len,rp,i);
				while(read[rp] == -1){
					rp++;
				}
			}
			add = 0;
		}else if(isalpha((int)ri->md[i])){
			//fprintf(stderr,"MD:%c\n",ri->md[i]);
			while(genome[gp] == -1){
				gp++;
			}
			genome[gp] = nuc_code[ri->md[i]];
			gp++;
			i++;
			if(!add){
				rp++;
			}
			
		}else{
			add = 1;
			i++;
		}
		if(i == l){
			exit_loop = 1;
			break;
		}
	}
	
	if(ri->strand[0] == 0){
		gp = 0;
		for(i =0;i < aln_len;i++){
			
			if(read[i] != -1 && genome[i] != -1){
				if(read[i] != genome[i]){
					seq_stats->mismatches[qual_key][gp][read[i]] += 1;
				}
				gp++;
				//			fprintf(stderr,"Mismatch %d\n",i);
			}else if(read[i] == -1 && genome[i] != -1){
				seq_stats->deletions[qual_key][gp] += 1;
				
				//			fprintf(stderr,"Deletion %d\n",i);
			}else if(read[i] != -1 && genome[i] == -1){
				seq_stats->insertions[qual_key][gp][read[i]] += 1;
				gp++;
				//			fprintf(stderr,"Insertion %d\n",i);
			}
		}
	}else{
		gp = ri->len-1;
		for(i = 0;i < aln_len;i++){
			
			if(read[i] != -1 && genome[i] != -1){
				if(read[i] != genome[i]){
					seq_stats->mismatches[qual_key][gp][reverse_int[read[i]]] += 1;
					//		fprintf(stderr,"Mismatch %d->%d\n",i,gp);
				}
				gp--;
				
			}else if(read[i] == -1 && genome[i] != -1){
				seq_stats->deletions[qual_key][gp] += 1;
				
				//	fprintf(stderr,"Deletion %d\n",i);
			}else if(read[i] != -1 && genome[i] == -1){
				seq_stats->insertions[qual_key][gp][reverse_int[read[i]]] += 1;
				gp--;
				//	fprintf(stderr,"Insertion %d\n",i);
			}
		}
	}
	
	free(read);
	free(genome);
	return aln_len;
}












