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


#define red 0xFF0000
#define orange 0xFF6600
#define lemon 0xFFFF00
#define green 0x009900
#define blue 0x3300FF 
#define lightblue 0x00CCFF
#define lightgrey 0xCCCCCC
#define grey 0x333333
#define black 0x000000
#define yellow 0xFFFC17
#define white 0xFFFFFF

void print_html_page(struct seq_stats* seq_stats,struct parameters* param,int file_num);

void print_header(FILE* out,char* lib_name);
void print_start_html(FILE* out,char* lib_name,struct seq_stats* seq_stats);
void print_footer(FILE* out);

void print_barchart_script(FILE* out);
void print_nucleotide_composition_canvas(FILE* out,struct seq_stats* seq_stats);
void print_nucleotide_composition_script(FILE* out,struct seq_stats* seq_stats);
void print_mapping_stats_canvas(FILE* out,struct seq_stats* seq_stats);
void print_mapping_stats_script(FILE* out,struct seq_stats* seq_stats);


void print_nucleotide_overrepresentation(FILE* out,struct seq_stats* seq_stats,struct parameters* param);
void print_top_overrepresentated(FILE* out,struct seq_stats* seq_stats,struct parameters* param);

void print_top_overrepresentated_long(FILE* out,struct seq_stats* seq_stats,struct parameters* param);

void print_length_distribution_script(FILE* out,struct seq_stats* seq_stats);
void print_length_distribution_canvas(FILE* out,struct seq_stats* seq_stats);

void print_quality_distribution_script(FILE* out,struct seq_stats* seq_stats,int solexa);
void print_quality_distribution_canvas(FILE* out,struct seq_stats* seq_stats);

void print_posteriors_distribution_script(FILE* out,struct seq_stats* seq_stats);
void print_posteriors_distribution_canvas(FILE* out,struct seq_stats* seq_stats);

void print_error_distribution_script(FILE* out,struct seq_stats* seq_stats);
void print_error_distribution_canvas(FILE* out,struct seq_stats* seq_stats);

void print_error_profile_canvas(FILE* out,struct seq_stats* seq_stats);
void print_error_profile_script(FILE* out,struct seq_stats* seq_stats);

void print_error_barchart_script(FILE* out);
void print_lineplot_script(FILE* out,struct seq_stats* seq_stats);






