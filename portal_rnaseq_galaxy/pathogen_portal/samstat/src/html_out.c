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
#include "misc.h"
#include "io.h"
#include "samstat.h"

#include "html_out.h"


//static char *q_legend[] = {"0.5 &lt;&#61;  p  &lt; 1", "0.1 &lt;&#61; p  &lt; 0.5 ", "1e-2 &lt;&#61; p  &lt; 0.1", "1e-3 &lt;&#61; p  &lt; 1e-2", "p &lt; 1e-3","Unmapped"};

//static char *q_legend2[] = {"0.5 <=  p < 1", "0.1 <= p < 0.5 ", "1e-2 <= p < 0.1", "1e-3 <= p < 1e-2", "p < 1e-3","Unmapped"};

static char *q_legend[] = {"MAPQ &lt; 3", "MAPQ &gt;= 3", "MAPQ &gt;= 10", "MAPQ &gt;= 20", "MAPQ &gt;= 30","Unmapped"};

static char *q_legend2[] = {"MAPQ < 3", "MAPQ >= 3", "MAPQ >= 10", "MAPQ >= 20", "MAPQ >= 30","Unmapped"};




void print_html_page(struct seq_stats* seq_stats,struct parameters* param,int file_num)
{
	
	
	int i;
	FILE* outfile;
	
	
	if(param->alt_lib_name){
		//fprintf(outfile,"%s\t",param->alt_lib_name);
		param->outfile = malloc(sizeof(char)*  (strlen(param->alt_lib_name)+ 1) );
		for(i = 0;i < strlen(param->alt_lib_name);i++){
			param->outfile[i] = param->alt_lib_name[i];
		}
		param->outfile[i+0] = 0;
		//param->outfile[i+0] = '.'; 
		//param->outfile[i+1] = 'h';
		//param->outfile[i+2] = 't';
		//param->outfile[i+3] = 'm';
		//param->outfile[i+4]  = 'l';
		//param->outfile[i+5]  = 0;
	}else if(file_num == -1 ){
		param->outfile = malloc(sizeof(char)*11 );

		param->outfile[0] = 's'; 
		param->outfile[1] = 't';
		param->outfile[2] = 'd';
		param->outfile[3] = 'i';
		param->outfile[4]  = 'n';
		param->outfile[5]  = '.';
		param->outfile[6]  = 'h';
		param->outfile[7]  = 't';
		param->outfile[8]  = 'm';
		param->outfile[9]  = 'l';
		param->outfile[10]  = 0;
	}else{
	
		param->outfile = malloc(sizeof(char)*  (strlen(param->infile[file_num])+ 7) );
		for(i = 0;i < strlen(param->infile[file_num]);i++){
			param->outfile[i] = param->infile[file_num][i];
		}
		param->outfile[i+0] = '.'; 
		param->outfile[i+1] = 'h';
		param->outfile[i+2] = 't';
		param->outfile[i+3] = 'm';
		param->outfile[i+4]  = 'l';
		param->outfile[i+5]  = 0;
	}
	
	if ((outfile = fopen(param->outfile, "w")) == NULL){
		fprintf(stderr,"can't open output\n");
		exit(-1);
	}
	
	//outfile = stdout;
	if(param->label){
		print_header(outfile,param->label );
	}
	else if(param->alt_lib_name){
		print_header(outfile,param->alt_lib_name );
	}else  if(file_num == -1 ){
		print_header(outfile,"stdin");
	}else{
		print_header(outfile,param->infile[file_num] );
	}
	print_mapping_stats_script(outfile,seq_stats);
	
	if(seq_stats->md){
		print_error_profile_script(outfile,seq_stats);
	}
	print_nucleotide_composition_script(outfile,seq_stats);
	
	print_length_distribution_script(outfile,seq_stats);
	
	if(param->print_qual ){
		print_quality_distribution_script(outfile,seq_stats,param->solexa );
	}
	
	if(param->print_posteriors ){
		print_posteriors_distribution_script(outfile,seq_stats);
	}
	
	
	if(seq_stats->sam){
		print_error_distribution_script(outfile,seq_stats);
	}
	
	//print_start_html(outfile,param->infile[file_num],seq_stats);
        if(param->label){
                print_start_html(outfile,param->label ,seq_stats);
        }	
	else if(param->alt_lib_name){
		print_start_html(outfile,param->alt_lib_name ,seq_stats);
	}else  if(file_num == -1 ){
		print_start_html(outfile,"stdin",seq_stats);
	}else{
		print_start_html(outfile,param->infile[file_num],seq_stats );
	}
	
	
	print_mapping_stats_canvas(outfile,seq_stats);
	
	if(param->print_qual ){
		print_quality_distribution_canvas(outfile,seq_stats);
	}
	if(param->print_posteriors ){
		print_posteriors_distribution_canvas(outfile,seq_stats);
	}
				
	if(seq_stats->md){
		print_error_profile_canvas(outfile,seq_stats);
	}
	print_nucleotide_overrepresentation(outfile,seq_stats,param);
	if(seq_stats->sam){
		print_error_distribution_canvas(outfile,seq_stats);
	}
	print_nucleotide_composition_canvas(outfile,seq_stats);
	print_length_distribution_canvas(outfile,seq_stats);
	
	
	print_top_overrepresentated(outfile,seq_stats,param);
	
	print_top_overrepresentated_long(outfile,seq_stats,param);

	print_footer(outfile);
	
	fclose(outfile);
	
	free(param->outfile);
	
	
}


void print_header(FILE* out,char* lib_name)
{
	fprintf(out,"<!DOCTYPE html>\n");
	fprintf(out,"<head>\n");
	fprintf(out,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/> \n");
	fprintf(out,"<title>Library %s</title>\n",shorten_pathname(lib_name));
	fprintf(out,"<meta name=\"keyword\" content=\"html5 library information\" />\n");
	fprintf(out,"<meta name=\"description\" content=\"site for experimenting with the features of html5\" />\n");
	fprintf(out,"<!--[if IE]>\n");
	fprintf(out,"<script src=\"http://html5shiv.googlecode.com/svn/trunk/html5.js\"></script>\n");
	fprintf(out,"<script src=\"j/excanvas.js\"></script>\n");
	fprintf(out,"<![endif]-->\n");
	fprintf(out,"<style>\n");
	fprintf(out,"#graphSpace { margin-left:80px; }\n");
	fprintf(out,"body { margin: 20px; font-family: arial,verdana,helvetica; background: #fff;}\n");
	fprintf(out,"h1 {color: rgb(36,89,158)}\n");
	fprintf(out,"h2 { clear:both;font-size: 140%%; font-weight:normal;  border-bottom: 1px solid rgb(0,0,0); }\n");
	fprintf(out,"canvas { margin-right: 20px; margin-bottom: 20px; }\n");
	fprintf(out,"pre { float : right; display:block; background: rgb(238,238,238); border: 1px dashed #666; padding: 15px 20px; margin: 0 0 10px 0;width:20%%;white-space:normal; }\n");
	fprintf(out,"help {float:right;	margin:0px;	width:auto;	height:auto;	border:1px dashed #66;	overflow:hidden;	background: rgb(238,238,238);padding: 1px;}\n");
	fprintf(out,"table {border-collapse: collapse;float:left;margin-bottom:20px;}\n");
	fprintf(out,"#merlist {border: 2px solid #000000; border-collapse: collapse;float:none;}\n");
	fprintf(out,"#merlist td {border: 1px solid #000000;width:auto;padding:5px;font-family:\"Courier New\" Courier 	monospace;font-size: 12pt;}\n");
	fprintf(out,"#merlist th {border: 2px solid #000000;width:auto;padding:5px;}\n");
	fprintf(out,"td {width:auto}\n");
	fprintf(out,"tr {text-align:right; font:  normal 10pt Arial;}\n");
	fprintf(out,"th {text-align:center;}\n");
	fprintf(out,"tr.top  {border-top: 2px solid #000000;}\n");
	
	
		
	
	fprintf(out,"table.mers {border-collapse:collapse;}\n");
	fprintf(out,"table.mers td  {width:auto;padding:5px;font-family:\"Courier New\" Courier 	monospace;font-size: 12pt;}\n");
	fprintf(out,"table.mers th  {width:auto;padding:5px;font-family:\"Courier New\" Courier 	monospace;font-size: 12pt;}\n");
	
	
	//fprintf(out,"table.mers.th, table.mers.th  {font-family:\"Courier New\" Courier 	monospace;font-size: 12pt;}\n");
	


	fprintf(out,"</style>\n");
	fprintf(out,"<script type=\"text/javascript\">\n");
	fprintf(out,"function draw() {\n");
}

void print_start_html(FILE* out,char* lib_name, struct seq_stats* seq_stats)
{
	fprintf(out,"}\n");
	
	print_barchart_script(out);
	print_lineplot_script(out,seq_stats);
	print_error_barchart_script(out);
	fprintf(out,"</script>\n");
	fprintf(out,"</head>\n");
	fprintf(out,"<body onLoad=\"draw();\">\n");
	fprintf(out,"<h1>%s</h1>\n",shorten_pathname(lib_name));
	
}

void print_footer(FILE* out)
{
	
	fprintf(out,"<br /><footer>Created by Samstat version %0.2f. Contact: timolassmann@gmail.com</footer>",VERSION);
	
	fprintf(out,"</body>\n");
	fprintf(out,"</html>\n");
}

void print_lineplot_script(FILE* out, struct seq_stats* seq_stats)
{
	float old_start,total;
	int i;
	fprintf(out,"function drawlineplot(context, data, startX, chartWidth, chartHeight,color,maxValue,xlegend) {\n");
	fprintf(out,"	chartWidth = chartWidth/2 ;\n");
	fprintf(out,"	var barWidth = (chartWidth-50) / (data.length+2);\n");
	fprintf(out,"	var plot_label = 1;\n");
	fprintf(out,"	startX = startX + 50;\n");
	fprintf(out,"	chartHeight = chartHeight -40;\n");
	fprintf(out,"	if(data.length > 100){\n");
	fprintf(out,"		plot_label = 10;	}else if (data.length > 20){\n");
	fprintf(out,"			plot_label = 5;	}else{\n");
	fprintf(out,"				plot_label = 1;	}\n");
	fprintf(out,"	context.textAlign = \"right\";\n");
	fprintf(out,"	context.font = \"8pt Arial\";\n");
	fprintf(out,"	context.beginPath();\n");
	fprintf(out,"	context.moveTo(40,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(40, 0);\n");
	fprintf(out,"	context.lineTo(45, 5);\n");
	fprintf(out,"	context.moveTo(40,0);\n");
	fprintf(out,"	context.lineTo(35, 5);\n");
	fprintf(out,"	context.moveTo(37,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4*3+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4*3+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/2+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/2+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4+20);\n");
	fprintf(out,"	context.moveTo(37,20);\n");
	fprintf(out,"	context.lineTo(43,20);\n");
	fprintf(out,"	context.strokeStyle = \"#000\";\n");
	fprintf(out,"	context.stroke();\n");
	fprintf(out,"	context.fillText(maxValue + \"\", 35, 20, 200);\n");
	fprintf(out,"	context.fillText(maxValue/4+ \"\", 35, 20+chartHeight/4*3, 200);\n");
	fprintf(out,"	context.fillText(maxValue/2+ \"\", 35, 20+chartHeight/2 , 200);\n");
	fprintf(out,"	context.fillText(maxValue/4*3+ \"\", 35, 20+chartHeight/4 , 200);\n");
	fprintf(out,"	context.fillText(0, 35, 20+chartHeight , 200);\n");
	fprintf(out,"	context.textAlign = \"center\";\n");
	fprintf(out,"	context.fillText(xlegend, (chartWidth-50) / 2 + 50, chartHeight + 10 +35, 200);\n");	
		
	fprintf(out,"	context.beginPath();\n");
	fprintf(out,"	context.moveTo(40,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(chartWidth, chartHeight+20);\n");
	fprintf(out,"	context.lineTo(chartWidth-5, chartHeight+15);\n");
	fprintf(out,"	context.moveTo(chartWidth, chartHeight+20);\n");
	fprintf(out,"	context.lineTo(chartWidth-5, chartHeight+25);\n");
	fprintf(out,"	context.strokeStyle = \"#000\";\n");
	fprintf(out,"	context.stroke();\n");
	fprintf(out,"	for (var i=0; i< data.length; i++) {\n");
	fprintf(out,"		old_height = -20;\n");
	fprintf(out,"		var values = data[i].split(\",\");\n");
	fprintf(out,"		var name = values[0];\n");
	fprintf(out,"		var height = parseInt(values[1]);\n");
	fprintf(out,"		if (parseInt(height) > parseInt(maxValue)) maxValue = height;\n");
	fprintf(out,"		context.fillStyle = color;\n");
	fprintf(out,"		if((i+1) %% plot_label == 0){\n");
	fprintf(out,"			context.textAlign = \"center\";\n");
	fprintf(out,"			context.fillStyle =\"#000\";\n");
	fprintf(out,"			context.font = \"8pt Arial\";\n");
	fprintf(out,"			context.fillText(i+1, startX + (i * barWidth) + i + barWidth/2, chartHeight + 10 +20, 200);	\n");
	fprintf(out,"		}\n");
	fprintf(out,"	}\n");
	fprintf(out,"	var test = data[0].split(\",\");\n");
	fprintf(out,"	for (var j = 1; j < test.length;j++){\n");
	fprintf(out,"		context.beginPath();\n");
	fprintf(out,"		for (var i=0; i< data.length; i++) {\n");
	fprintf(out,"			old_height = -20;\n");
	fprintf(out,"			var values = data[i].split(\",\");\n");
	fprintf(out,"			var name = values[0];\n");
	fprintf(out,"			var height = parseInt(values[j]);\n");
	fprintf(out,"			if (parseInt(height) > parseInt(maxValue)) maxValue = height;\n");
	fprintf(out,"			context.fillStyle = color;\n");
	fprintf(out,"			context.lineTo(startX + (i * barWidth) + i,chartHeight -  height / maxValue *chartHeight - old_height);\n");
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 1){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",blue );
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 2){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",green);
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 3){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",lemon);
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 4){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",orange);
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 5){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",red);
	fprintf(out,"		}\n");
	fprintf(out,"		if(j == 6){\n");
	fprintf(out,"			context.strokeStyle = \"#%06x\";\n",grey);
	fprintf(out,"		}\n");
	fprintf(out,"		context.stroke();\n");
	fprintf(out,"	}	\n");
	
	total = 0.0;
	for(i= 0;i < 6;i++){
		total += seq_stats->alignments[i];
	}
	
	old_start =10;
	fprintf(out,"context.textAlign = \"left\";\n");
	fprintf(out,"context.font = \"12pt Arial\";\n");
	for(i= 4;i >=0;i--){
		switch (i) {
			case 0:
				fprintf(out,"context.fillStyle = \"#%06x\"\n",blue);
				break;
			case 1:
				fprintf(out,"context.fillStyle = \"#%06x\"\n",green);
				break;
			case 2:
				fprintf(out,"context.fillStyle = \"#%06x\"\n",lemon);
				break;
			case 3:
				fprintf(out,"context.fillStyle = \"#%06x\"\n",orange);
				break;
			case 4:
				fprintf(out,"context.fillStyle = \"#%06x\"\n",red);
				break;
		}
		
		fprintf(out,"context.fillRect (chartWidth+20, %f, 20, 20);",old_start); 
		fprintf(out,"context.fillStyle = \"#%06x\"\n",black);
		fprintf(out,"context.fillText(\"%s (%2.1f%% , %d)\", chartWidth+50, %f);\n",q_legend2[i], (float)seq_stats->alignments[i] / total *100.0f,seq_stats->alignments[i], old_start + 15);
		
		
		
		old_start += 35;
		
	}
	fprintf(out,"context.fillStyle = \"#%x\"\n",grey);
	fprintf(out,"context.fillRect (chartWidth+20, %f, 20, 20);",old_start); 
	fprintf(out,"context.fillStyle = \"#%06x\"\n",black);
	fprintf(out,"context.fillText(\"Unmapped (%2.1f%% , %d)\", chartWidth+50, %f);\n", (float)seq_stats->alignments[5] / total *100.0f,seq_stats->alignments[5], old_start + 15);
	
	
	fprintf(out,"}	\n");
}


void print_barchart_script(FILE* out)
{
	
	fprintf(out,"function drawBarChart(context, data, startX, chartWidth, chartHeight,color,maxValue,xlegend) {\n");
	fprintf(out,"	var barWidth = (chartWidth-50) / (data.length+2);\n");
	fprintf(out,"	var plot_label = 1;\n");
	fprintf(out,"	startX = startX + 50;\n");
	fprintf(out,"	chartHeight = chartHeight -40;\n");
	fprintf(out,"	if(data.length > 100){\n");
	fprintf(out,"		plot_label = 10;");
	fprintf(out,"	}else if (data.length > 20){\n");
	fprintf(out,"		plot_label = 5;");
	fprintf(out,"	}else{\n");
	fprintf(out,"		plot_label = 1;");
	fprintf(out,"	}\n");
	
	fprintf(out,"	context.textAlign = \"right\";\n");
	fprintf(out,"	context.font = \"8pt Arial\";\n");
	fprintf(out,"	context.beginPath();\n");
	fprintf(out,"	context.moveTo(40,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(40, 0);\n");
	fprintf(out,"	context.lineTo(45, 5);\n");
	fprintf(out,"	context.moveTo(40,0);\n");
	fprintf(out,"	context.lineTo(35, 5);\n");
	fprintf(out,"	context.moveTo(37,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4*3+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4*3+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/2+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/2+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4+20);\n");
	fprintf(out,"	context.moveTo(37,20);\n");
	fprintf(out,"	context.lineTo(43,20);\n");
	fprintf(out,"	context.strokeStyle = \"#000\";\n");
	fprintf(out,"	context.stroke();\n");
	fprintf(out,"	context.fillText(maxValue + \"k\", 35, 20, 200);\n");
	fprintf(out,"	context.fillText(maxValue/4+ \"k\", 35, 20+chartHeight/4*3, 200);\n");
	fprintf(out,"	context.fillText(maxValue/2+ \"k\", 35, 20+chartHeight/2 , 200);\n");
	fprintf(out,"	context.fillText(maxValue/4*3+ \"k\", 35, 20+chartHeight/4 , 200);\n");
	fprintf(out,"	context.fillText(0, 35, 20+chartHeight , 200);\n");
	fprintf(out,"	context.textAlign = \"center\";\n");
	fprintf(out,"	context.fillText(xlegend, (chartWidth-50) / 2 + 50, chartHeight + 10 +35, 200);	\n");
	fprintf(out,"	for (var i=0; i< data.length; i++) {\n");
	fprintf(out,"		old_height = -20;\n");
	fprintf(out,"		var values = data[i].split(\",\");\n");
	fprintf(out,"		var name = values[0];\n");
	//fprintf(out,"		var height = parseInt(values[1]);\n");
	fprintf(out,"		var height = values[1];\n");
	fprintf(out,"		if (parseInt(height) > parseInt(maxValue)) maxValue = height;\n");
	//fprintf(out,"		if(maxValue == values[1]){\n");
	//fprintf(out,"			context.fillStyle = \"#990000\";\n");
	//fprintf(out,"		}else{\n");
	fprintf(out,"			context.fillStyle = color;\n");
	//fprintf(out,"		}\n");
	fprintf(out,"		drawRectangle(context, startX + (i * barWidth) + i,chartHeight -  height / maxValue *chartHeight - old_height, barWidth, height / maxValue *chartHeight , true);\n");
	fprintf(out,"		if((i+1) %% plot_label == 0){\n");
	fprintf(out,"			context.textAlign = \"center\";\n");
	fprintf(out,"			context.fillStyle =\"#000\";\n");
	fprintf(out,"			context.font = \"8pt Arial\";\n");
	fprintf(out,"			context.fillText(name, startX + (i * barWidth) + i + barWidth/2, chartHeight + 10 +20, 200);	\n");	
	fprintf(out,"		}\n");
	fprintf(out,"	}\n");
	fprintf(out,"}	\n");	
	
	fprintf(out,"function drawRectangle(contextO, x, y, w, h, fill) {	\n");		
	fprintf(out,"	contextO.beginPath();\n");
	fprintf(out,"	contextO.rect(x, y, w, h);\n");
	fprintf(out,"	contextO.closePath();\n");
	fprintf(out,"	contextO.stroke();\n");
	fprintf(out,"	if (fill) contextO.fill();\n");
	fprintf(out,"}	\n");
}


void print_error_barchart_script(FILE* out)
{	
	fprintf(out,"function drawBarChart2(context, data, startX, chartWidth, chartHeight,maxValue) {;\n");
	fprintf(out,"	var barWidth = (chartWidth-50) / (data.length+2);\n");
	fprintf(out,"	var plot_label = 1;\n");
	fprintf(out,"	var old_height = 0;\n");
	fprintf(out,"	startX = startX + 50;\n");
	fprintf(out,"	chartHeight = chartHeight -40;\n");
	fprintf(out,"	if(data.length > 100){\n");
	fprintf(out,"		plot_label = 10;\n");
	fprintf(out,"	}else if (data.length > 20){\n");
	fprintf(out,"		plot_label = 5;\n");
	fprintf(out,"	}else{\n");
	fprintf(out,"		plot_label = 1;");
	fprintf(out,"	}\n");
	
	fprintf(out,"	context.textAlign = \"right\";\n");
	fprintf(out,"	context.font = \"8pt Arial\";\n");
	fprintf(out,"	context.beginPath();\n");
	fprintf(out,"	context.moveTo(40,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(40, 0);\n");
	fprintf(out,"	context.lineTo(45, 5);\n");
	fprintf(out,"	context.moveTo(40,0);\n");
	fprintf(out,"	context.lineTo(35, 5);\n");
	fprintf(out,"	context.moveTo(37,chartHeight+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4*3+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4*3+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/2+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/2+20);\n");
	fprintf(out,"	context.moveTo(37,chartHeight/4+20);\n");
	fprintf(out,"	context.lineTo(43,chartHeight/4+20);\n");
	fprintf(out,"	context.moveTo(37,20);\n");
	fprintf(out,"	context.lineTo(43,20);\n");
	fprintf(out,"	context.strokeStyle = \"#000\";\n");
	fprintf(out,"	context.closePath();\n");
	fprintf(out,"	context.stroke();\n");

	
	fprintf(out,"	context.fillText(maxValue+ \"k\", 35, 20, 200);\n");
	fprintf(out,"	context.fillText(maxValue/4+ \"k\", 35, 20+chartHeight/4*3, 200);\n");
	fprintf(out,"	context.fillText(maxValue/2+ \"k\", 35, 20+chartHeight/2 , 200);\n");
	fprintf(out,"	context.fillText(maxValue/4*3+ \"k\", 35, 20+chartHeight/4 , 200);\n");
	fprintf(out,"	context.fillText(0, 35, 20+chartHeight , 200);\n");
	fprintf(out,"	context.textAlign = \"center\";\n");
	fprintf(out,"	context.fillText(\"Position\", (chartWidth-50) / 2 + 50, chartHeight + 10 +35, 200);	\n");
	
	
	fprintf(out,"	context.font = \"12pt Arial\";\n");
	fprintf(out,"	context.fillStyle = \"#%06x\";\n",green);
	fprintf(out,"	context.fillRect(chartWidth - 100, 0, 20 , 20 );\n");
	
	fprintf(out,"	context.fillStyle = \"#%06x\";\n",black);
	fprintf(out,"	context.fillText(\"A\",chartWidth - 100 +10, 15);\n");

	
	fprintf(out,"	context.fillStyle = \"#%06x\";\n",blue );
	fprintf(out,"	context.fillRect(chartWidth - 70, 0, 20 , 20 );\n");

	fprintf(out,"	context.fillStyle = \"#%06x\";\n",white);
	fprintf(out,"	context.fillText(\"C\",chartWidth - 70+10, 15);\n");

	fprintf(out,"	context.fillStyle = \"#%06x\";\n",yellow);
	fprintf(out,"	context.fillRect(chartWidth - 40, 0, 20 , 20 );\n");
	
	fprintf(out,"	context.fillStyle = \"#%06x\";\n",black);
	fprintf(out,"	context.fillText(\"G\",chartWidth - 40+10, 15);\n");

	fprintf(out,"	context.fillStyle = \"#%06x\";\n",red );
	fprintf(out,"	context.fillRect(chartWidth - 10, 0, 20 , 20 );\n");

	fprintf(out,"	context.fillStyle = \"#%06x\";\n",black);
	fprintf(out,"	context.fillText(\"T\",chartWidth - 10+10, 15);\n");

	
	fprintf(out,"	context.strokeStyle = \"#000\";\n");
	fprintf(out,"	context.font = \"8pt Arial\";\n");
	//A red, T blue, C yellow, G green, and U cyan.
	fprintf(out,"	for (var i=0; i< data.length; i++) {\n");
	fprintf(out,"		old_height = -20;\n");
	fprintf(out,"		for (var j=0; j< 4; j++) {\n");
	fprintf(out,"			\n");
	fprintf(out,"			\n");
	fprintf(out,"			if(j == 0){\n");
	fprintf(out,"				context.fillStyle = \"#%06x\";\n",green);
	fprintf(out,"			}\n");
	fprintf(out,"			if(j == 1){\n");
	fprintf(out,"				context.fillStyle = \"#%06x\";\n",blue );
	fprintf(out,"			}\n");
	fprintf(out,"			if(j == 2){\n");
	fprintf(out,"				context.fillStyle = \"#%06x\";\n",yellow);
	fprintf(out,"			}\n");
	fprintf(out,"			if(j == 3){\n");
	fprintf(out,"				context.fillStyle = \"#%06x\";\n",red );
	fprintf(out,"			}\n");
	fprintf(out,"			\n");
	fprintf(out,"			var height = data[i][j];\n");
	fprintf(out,"			if (parseInt(height) > parseInt(maxValue)) maxValue = height;\n");
	fprintf(out,"			\n");
	fprintf(out,"			drawRectangle(context, startX + (i * barWidth) + i,chartHeight  -  height / maxValue *chartHeight - old_height, barWidth, height / maxValue *chartHeight , true);\n");
	fprintf(out,"			old_height = old_height +height / maxValue *chartHeight  ; \n");
	fprintf(out,"		}\n");
	fprintf(out,"		if((i+1) %% plot_label == 0){\n");
	fprintf(out,"			context.fillStyle =\"#000\";\n");
	fprintf(out,"			context.fillText(i+1, startX + (i * barWidth) + i + barWidth/2, chartHeight + 10 + 20, 200);\n");
	fprintf(out,"		}\n");
	fprintf(out,"	}\n");
	fprintf(out,"}	\n");	
}


void print_top_overrepresentated_long(FILE* out,struct seq_stats* seq_stats,struct parameters* param)
{
	char nuc[] = "ACGTN";
	float total;
	int i,j,g;
	
	total = 0.0f;
	

	double* mer_sum = malloc(sizeof(double)* KMERALLOC);
	double sum = 0.0f;
	double sum_Q[6];
	for(j = 0;j < 6;j++){
		sum_Q[j] = 0;
		for(i =0; i < KMERALLOC;i++){
			sum_Q[j] += seq_stats->ten_mers[j][i]->count;
		}
	}
	
	for(i =0; i < KMERALLOC;i++){
		mer_sum[i]  = 0;
		for(j = 0;j < 6;j++){
			mer_sum[i] += seq_stats->ten_mers[j][i]->count;
			sum += seq_stats->ten_mers[j][i]->count;
		}
	}
	
	for(i = 0; i < KMERALLOC;i++){
		mer_sum[i] /= sum;
		for(j = 0;j < 6;j++){
			if(seq_stats->alignments[j] &&seq_stats->ten_mers[j][i]->count ){
				//if(seq_stats->ten_mers[j][i]->count == 2431){
				//fprintf(stderr,"%d	%f	%d	%f	",seq_stats->ten_mers[j][i]->count,sum_Q[j]  ,seq_stats->ten_mers[j][i]->identifier,mer_sum[i]);
				//}
				//fprintf(stderr,"tenmers: %d    %f      %d      %f     \n",seq_stats->ten_mers[j][i]->count,sum_Q[j] ,seq_stats->ten_mers[j][i]->count,mer_sum[i]);
				seq_stats->ten_mers[j][i]->p_value = betai(seq_stats->ten_mers[j][i]->count,sum_Q[j] - seq_stats->ten_mers[j][i]->count + 1, mer_sum[i]) * KMERALLOC;
				
				if(seq_stats->ten_mers[j][i]->p_value > 1.0f){
					seq_stats->ten_mers[j][i]->p_value = 1.0f;
				}
				if(seq_stats->ten_mers[j][i]->p_value < 0.0f){
					seq_stats->ten_mers[j][i]->p_value = 0.0f;
				}
				
				//if(seq_stats->ten_mers[j][i]->count == 2431){
				//fprintf(stderr,"%f\n",seq_stats->ten_mers[j][i]->p_value);
				//}
			}else{
				seq_stats->ten_mers[j][i]->p_value = 1.0f;
			}
		}
	}
	for(i = 5;i >= 0;i--){ 
		if(seq_stats->alignments[i] ){
			qsort((void *) seq_stats->ten_mers[i],KMERALLOC,sizeof(struct mer_info*),cmpr); 
		}
	}
	
	
	
	fprintf(out,"<section>\n");
		
	fprintf(out,"<h2>Top 20 over-represented %d-mers</h2>\n", KMERLEN);
		
		
	fprintf(out,"<table id=\"merlist\" >\n");
		
	fprintf(out,"<tr>");
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i] ){
			fprintf(out,"<th COLSPAN=3 style=\"text-align:center\" >%s</th>\n",q_legend[i]);
		}
	}
	if(seq_stats->alignments[5] ){
		fprintf(out,"<th COLSPAN=3 style=\"text-align:center\">%s</th>\n",q_legend[5]);
	}
	fprintf(out,"<tr>\n");
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i] ){
			fprintf(out,"<th>Seqence</th>");
			fprintf(out,"<th>Count</th>");
			fprintf(out,"<th>P-value</th>");
		}
	}	
	
	if(seq_stats->alignments[5] ){
		fprintf(out,"<th>Seqence</th>");
		fprintf(out,"<th>Count</th>");
		fprintf(out,"<th>P-value</th>");
	}
	
	fprintf(out,"</tr>\n");
	
		
	for(j = 0; j < 20;j++){
		fprintf(out,"<tr>\n");
		for(i = 4;i >= 0;i--){ 
			if(seq_stats->alignments[i]){
				
				fprintf(out,"<td style=\"border-left: 2px solid #000000;\">\n");
				for(g = 0; g < KMERLEN;g++){
					
					
					fprintf(out,"%c",nuc[(  (seq_stats->ten_mers[i][j]->identifier >> ((KMERLEN*2 - 2 ) - (g*2))) & 0x3)]);
				}
				fprintf(out,"</td>\n");
				fprintf(out,"<td>\n");
				fprintf(out,"%d",seq_stats->ten_mers[i][j]->count);
				fprintf(out,"</td>\n");
				
				fprintf(out,"<td>\n");
				fprintf(out,"%0.1e",seq_stats->ten_mers[i][j]->p_value);
				fprintf(out,"</td>\n");
				
				
			}
		}
		if(seq_stats->alignments[5]){
			
			fprintf(out,"<td style=\"border-left: 2px solid #000000;\">\n");
			for(g = 0; g < KMERLEN;g++){
				fprintf(out,"%c",nuc[(  (seq_stats->ten_mers[5][j]->identifier >> ((KMERLEN*2 - 2 ) - (g*2))) & 0x3)]);
			}
			fprintf(out,"</td>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"%d",seq_stats->ten_mers[5][j]->count);
			fprintf(out,"</td>\n");
			
			fprintf(out,"<td>\n");
			fprintf(out,"%0.1e",seq_stats->ten_mers[5][j]->p_value);
			fprintf(out,"</td>\n");
			
			
		}
		
		fprintf(out,"</tr>\n");
	}
		
	fprintf(out,"</table>\n");
	fprintf(out,"</section>\n");
	//}
	
	free(mer_sum);
}



void print_top_overrepresentated(FILE* out,struct seq_stats* seq_stats,struct parameters* param)
{
	int i,j,c,g;
	
	
	int max_len[6];
	int min_len[6];
	
	double total = 0;
	double p_value = 0.0f;
	int num_kmers = 1 <<(param->kmer_size *2) ;
	char nuc[] = "ACGTN";
	
	for(c= 0;c < 6;c++){
		
		max_len[c] = -1;
		min_len[c] =  MAX_SEQ_LEN+10;
		
		for(i = 0; i < MAX_SEQ_LEN;i++){
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len[c] ){
					max_len[c] = i;
				}
				
				if(i < min_len[c]){
					min_len[c] =  i;
				}
			}		
		}
		
		if(min_len[c] ==  MAX_SEQ_LEN+10){
			min_len[c] = 0;
		}
		
		if(max_len[c] == -1){
			max_len[c] = 0;
		}
	}	
	
	float* p_nuc = malloc(sizeof(float)*num_kmers );
	
	for(i = 0; i < num_kmers;i++){
		p_nuc[i] = 0.0f;
	}
	
	for(i = 0;i  <  6;i++){ 
		for(j = 0; j < num_kmers;j++){
			for(c= 0; c < max_len[i]- (param->kmer_size -1);c++){
				p_nuc[j] += seq_stats->kmers[i][c][j];
				total += seq_stats->kmers[i][c][j];
			}
		}
	}
	
	for(i = 0; i < num_kmers;i++){
		p_nuc[i] = p_nuc[i] / total;
	}
	
	struct mer_info** mi  = malloc(sizeof(struct mer_info*)* num_kmers);
	for(i = 0; i < num_kmers;i++){
		mi[i] = malloc(sizeof(struct mer_info));
	}
	
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i]){
			for(c = 0; c < num_kmers;c++){
				mi[c]->count = 0;
				mi[c]->p_value = 0;
				mi[c]->identifier = 0;
			}
			fprintf(out,"<section>\n");
			
			fprintf(out,"<h2>Top 5 over-represented %d-mers %s</h2>\n", param->kmer_size,q_legend[i]);
			
			
			fprintf(out,"<table class=\"mers\" >\n");
			
			fprintf(out,"<tr>\n");
			fprintf(out,"<th>Pos</th>");
			for(j = 0; j < min_len[i]- (param->kmer_size -1);j++){
				fprintf(out,"<th>%d</th>",j+1);
			}
			fprintf(out,"</tr>\n");
			
			fprintf(out,"<tr>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"Seq\n");
			fprintf(out,"</td>\n");
			for(c= 0; c < min_len[i]- (param->kmer_size -1);c++){
				for(j = 0; j < num_kmers;j++){
					if(seq_stats->kmers[i][c][j]){
						p_value = betai(seq_stats->kmers[i][c][j],seq_stats->alignments[i] - seq_stats->kmers[i][c][j] +1, p_nuc[j]) * (double) min_len[i] * (double) num_kmers;
						if(p_value > 1.0){
							p_value = 1.0;
						}					
						
					}else{
						p_value = 1.0;
					}
					mi[j]->p_value = p_value;
					mi[j]->count = seq_stats->kmers[i][c][j];
					mi[j]->identifier = j;
				}
				qsort((void *) mi,num_kmers,sizeof(struct mer_info*),cmpr); 
				
				fprintf(out,"<td>\n");
				for(j = 0; j < 5;j++){
					if(mi[j]->p_value <= 1e-100){
						fprintf(out,"<p style = \"color: #%06x;  \" >\n",red);
					}else if (mi[j]->p_value <= 1e-20){
						fprintf(out,"<p style = \"color: #%06x;  \" >\n",orange);
					}else if (mi[j]->p_value <= 1e-10){
						fprintf(out,"<p style = \"color: #%06x;  \" >\n",green);
					}else if (mi[j]->p_value <= 1e-5){
						fprintf(out,"<p style = \"color: #%06x;  \" >\n",blue);
					}else{
						fprintf(out,"<p>");
						
					}
					
					//fprintf(stderr,"column:%d	",c);
					for(g = 0; g < param->kmer_size;g++){
						fprintf(out,"%c",nuc[(  (mi[j]->identifier >> ((param->kmer_size*2 - 2 ) - (g*2))) & 0x3)]);
					}
					fprintf(out,"</p>");//,mi[j]->count);
				}
				fprintf(out,"</td>\n");
				
				
			}
			fprintf(out,"</tr>\n");
			fprintf(out,"</table>\n");
			fprintf(out,"<pre>\n");
			fprintf(out,"Top 5 enriched %d-mers at each positon within reads are shown. Red, orange, green and blue corresponds to p-values 1e-100, 1e-20, 1e-10 and 1e-5 respectively.\n",param->kmer_size);
			fprintf(out,"</pre>");
			fprintf(out,"</section>\n");
		}
	}
	//unmapped
	i = 5;
	if(seq_stats->alignments[i]){
		for(c = 0; c < num_kmers;c++){
			mi[c]->count = 0;
			mi[c]->p_value = 0;
			mi[c]->identifier = 0;
		}
		fprintf(out,"<section>\n");
		
		fprintf(out,"<h2>Top 5 over-represented %d-mers %s</h2>\n", param->kmer_size,q_legend[5]);
		
		
		fprintf(out,"<table class=\"mers\" >\n");
		
		fprintf(out,"<tr>\n");
		fprintf(out,"<th>Pos</th>");
		for(j = 0; j < min_len[i]- (param->kmer_size -1);j++){
			fprintf(out,"<th>%d</th>",j+1);
		}
		fprintf(out,"</tr>\n");
		
		fprintf(out,"<tr>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"Seq\n");
		fprintf(out,"</td>\n");
		for(c= 0; c < min_len[i]- (param->kmer_size -1);c++){
			for(j = 0; j < num_kmers;j++){
				if(seq_stats->kmers[i][c][j]){
					
					p_value = betai(seq_stats->kmers[i][c][j],seq_stats->alignments[i] - seq_stats->kmers[i][c][j] + 1, p_nuc[j]) * (double) min_len[i] * (double) num_kmers;
					if(p_value > 1.0){
						p_value = 1.0;
					}					
					
				}else{
					p_value = 1.0;
				}
				mi[j]->p_value = p_value;
				mi[j]->count = seq_stats->kmers[i][c][j];
				mi[j]->identifier = j;
			}
			qsort((void *) mi,num_kmers,sizeof(struct mer_info*),cmpr); 
			
			fprintf(out,"<td>\n");
			for(j = 0; j < 5;j++){
				if(mi[j]->p_value <= 1e-100){
					fprintf(out,"<p style = \"color: #%06x;  \" >\n",red);
				}else if (mi[j]->p_value <= 1e-20){
					fprintf(out,"<p style = \"color: #%06x;  \" >\n",orange);
				}else if (mi[j]->p_value <= 1e-10){
					fprintf(out,"<p style = \"color: #%06x;  \" >\n",green);
				}else if (mi[j]->p_value <= 1e-5){
					fprintf(out,"<p style = \"color: #%06x;  \" >\n",blue);
				}else{
					fprintf(out,"<p>");
					
				}
				
				//fprintf(stderr,"column:%d	",c);
				for(g = 0; g < param->kmer_size;g++){
					fprintf(out,"%c",nuc[(  (mi[j]->identifier >> ((param->kmer_size*2 - 2 ) - (g*2))) & 0x3)]);
				}
				fprintf(out,"</p>");//,mi[j]->count);
			}
			fprintf(out,"</td>\n");
			
			
		}
		fprintf(out,"</tr>\n");
		fprintf(out,"</table>\n");
		fprintf(out,"<pre>\n");
		fprintf(out,"Top 5 enriched %d-mers at each positon withion reads are shown. Red, orange, green and blue corresponds to p-values 1e-100, 1e-20, 1e-10 and 1e-5 respectively.\n",param->kmer_size);
		fprintf(out,"</pre>");
		fprintf(out,"</section>\n");
	}
	
	
	for(i = 0; i < num_kmers;i++){
		free(mi[i]); 
	}
	free(mi);
	
	free(p_nuc);	
	
}

void print_nucleotide_overrepresentation(FILE* out,struct seq_stats* seq_stats,struct parameters* param)
{
	int i,j,c,g;

	
	int max_len[6];
	int min_len[6];
	
	float total = 0.0f;
	int div;
	double p_value = 0.0f;
	double tmp = 0.0f;
	int num_kmers = 1 <<(param->kmer_size *2) ;
	char nuc[] = "ACGTN";
	
	for(c= 0;c < 6;c++){
		
		max_len[c] = -1;
		min_len[c] =  MAX_SEQ_LEN+10;
		
		for(i = 0; i < MAX_SEQ_LEN;i++){
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len[c] ){
					max_len[c] = i;
				}
				
				if(i < min_len[c]){
					min_len[c] =  i;
				}
			}		
		}
		
		if(min_len[c] ==  MAX_SEQ_LEN+10){
			min_len[c] = 0;
		}
		
		if(max_len[c] == -1){
			max_len[c] = 0;
		}
	}	

	
	float* p_nuc = malloc(sizeof(float)*num_kmers );
	
	for(i = 0; i < num_kmers;i++){
		p_nuc[i] = 0.0f;
	}
	
	for(i = 0;i  <  6;i++){ 
		for(j = 0; j < num_kmers;j++){
			for(c= 0; c < max_len[i]- (param->kmer_size -1);c++){
				p_nuc[j] += seq_stats->kmers[i][c][j];
				total += seq_stats->kmers[i][c][j];
			}
		}
	}
	
	for(i = 0; i < num_kmers;i++){
		p_nuc[i] = p_nuc[i] / total;
	}
	//fprintf(out,"<section>\n");
	div = 1000;
	fprintf(out,"<section>\n");
	fprintf(out,"<h2>Nucleotide Over-representation along read</h2>\n");
	fprintf(out,"<table style = \"border-collapse: collapse;margin-bottom:20px;float:none;\">\n");
	fprintf(out,"<tr>\n");
	fprintf(out,"<th>\n");
	fprintf(out,"Mer");
	fprintf(out,"</th>\n");
	fprintf(out,"<th style = \"width:100px\">\n");
	fprintf(out,"Q");
	fprintf(out,"</th>\n");
	int min = 1000000000;
	for(i = 0;i < 6;i++){ 
		if(seq_stats->alignments[i]){
			//fprintf(stderr,"%d	%d\n",min_len[i] ,max_len[i]);
			if(min_len[i] < min){
				min = min_len[i];
			}
		}
	}

	for(j = 0; j < min- (param->kmer_size -1);j++){
		fprintf(out,"<th>%d</th>",j+1);
	}
	fprintf(out,"</tr>\n");
	
	for(j = 0; j < num_kmers;j++){
		for(i = 4;i >= 0;i--){ 
			if(seq_stats->alignments[i]){
				if(i == 4){
					fprintf(out,"<tr class=\"top\" >\n");
				}else{
					fprintf(out,"<tr>\n");
				}
				fprintf(out,"<td>\n");
				for(c = 0; c < param->kmer_size;c++){
					g =  (j >> ((param->kmer_size*2 - 2 ) - (c*2))) & 0x3;
					fprintf(out,"%c",nuc[g]);
				}
				
				fprintf(out,"</td><td>%s</td>",q_legend[i]);
				
				//	if(seq_stats->alignments[i]){
				
				for(c= 0; c < min- (param->kmer_size -1);c++){
					
					
					tmp = (float)seq_stats->kmers[i][c][j] / (float)seq_stats->alignments[i] * 100;
					
					if(tmp >= 50){
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",red);
					}else if (tmp >= 40){
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",orange);
					}else if (tmp >= 30){
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",lemon);
					}else if (tmp >= 20){
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",green);
					}else if (tmp >= 10){
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",lightblue);
					}else{
						fprintf(out,"<td style = \"background: #%06x;  \" >\n",lightgrey);
						
					}
					
					
					if(seq_stats->kmers[i][c][j]){
						
						
						
						p_value = betai(seq_stats->kmers[i][c][j],seq_stats->alignments[i] - seq_stats->kmers[i][c][j] + 1, p_nuc[j]) * (double) min_len[i] * (double) num_kmers;
						if(p_value > 1.0){
							p_value = 1.0;
						}
						
						//fprintf(stderr,"%0.2e\t",p_value);
						
						
						
					}else{
						p_value = 1.0;
						//fprintf(stderr,"1.0\t");
					}
					
					
					
					if(p_value <= 1e-100){
						fprintf(out,"<b>% 2.1f</b>",tmp );
					}else{
						fprintf(out,"% 2.1f",tmp );
					}
					fprintf(out,"</td>");
				}
				
				fprintf(out,"</tr>\n");
			}
		}
		//unamppes...
		i = 5;
		if(seq_stats->alignments[i]){
			if(i == 4){
				fprintf(out,"<tr class=\"top\" >\n");
			}else{
				fprintf(out,"<tr>\n");
			}
			fprintf(out,"<td>\n");
			for(c = 0; c < param->kmer_size;c++){
				g =  (j >> ((param->kmer_size*2 - 2 ) - (c*2))) & 0x3;
				fprintf(out,"%c",nuc[g]);
			}
			
			fprintf(out,"</td><td>%s</td>",q_legend[5]);
			
			//	if(seq_stats->alignments[i]){
			
			for(c= 0; c < min- (param->kmer_size -1);c++){
				
				tmp = (float)seq_stats->kmers[i][c][j] / (float)seq_stats->alignments[i] * 100;
				
				if(tmp >= 50){
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",red);
				}else if (tmp >= 40){
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",orange);
				}else if (tmp >= 30){
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",lemon);
				}else if (tmp >= 20){
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",green);
				}else if (tmp >= 10){
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",lightblue);
				}else{
					fprintf(out,"<td style = \"background: #%06x;  \" >\n",lightgrey);
				}
				
				
				if(seq_stats->kmers[i][c][j]){
					p_value = betai(seq_stats->kmers[i][c][j],seq_stats->alignments[i] - seq_stats->kmers[i][c][j] + 1, p_nuc[j]) * (double) min_len[i] * (double) num_kmers;
					if(p_value > 1.0){
						p_value = 1.0;
					}
					
					//fprintf(stderr,"%0.2e\t",p_value);
					
					
					
				}else{
					p_value = 1.0;
					//fprintf(stderr,"1.0\t");
				}
				
				
				
				if(p_value <= 1e-100){
					fprintf(out,"<b>%0.1f</b>",tmp );
				}else{
					fprintf(out,"%0.1f",tmp );
				}
				fprintf(out,"</td>");
				
			}
			
			fprintf(out,"</tr>\n");
		}
		
		
	}
	fprintf(out,"</table>\n");
	fprintf(out,"<pre>\n");
	fprintf(out,"Percentage of %d-mers at each position of the read for each alignment quality interval. Numbers shown in bold corresponds to p-values of &lt;&#61; 1e-100.\n",param->kmer_size);
	fprintf(out,"</pre>");
	fprintf(out,"</section>\n");
	free(p_nuc);	
}



void print_nucleotide_composition_canvas(FILE* out,struct seq_stats* seq_stats)
{
	int i;
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i]){
		fprintf(out,"<section>\n");
		
		fprintf(out,"<h2>Nucleotide Composition %s</h2>\n",q_legend[i] );
		fprintf(out,"<table>\n");
		
		fprintf(out,"<tr style = \"text-align:center; font:  bold 12pt Arial;\">\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",green);
		fprintf(out,"A");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",blue);
		fprintf(out,"C");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",yellow);
		fprintf(out,"G");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",red);
		fprintf(out,"T");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",grey);
		fprintf(out,"N");
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		
		
		fprintf(out,"<tr>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_0_canvas\" width=\"200\" height=\"100\"></canvas>\n",i);
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_1_canvas\" width=\"200\" height=\"100\"></canvas>\n",i);
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_2_canvas\" width=\"200\" height=\"100\"></canvas>\n",i);
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_3_canvas\" width=\"200\" height=\"100\"></canvas>\n",i);
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_4_canvas\" width=\"200\" height=\"100\"></canvas>\n",i);
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		fprintf(out,"</table>\n");
		
		fprintf(out,"</section>\n");
		}
	}
	if(seq_stats->alignments[5]){
		fprintf(out,"<section>\n");
		
		fprintf(out,"<h2>Nucleotide Composition %s</h2>\n",q_legend[5]);
		fprintf(out,"<table>\n");
		
		fprintf(out,"<tr style = \"text-align:center; font:  bold 12pt Arial;\">\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",green);
		fprintf(out,"A");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",blue);
		fprintf(out,"C");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",yellow);
		fprintf(out,"G");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",red);
		fprintf(out,"T");
		fprintf(out,"</td>\n");
		fprintf(out,"<td style = \"color: #%06x;  \" >\n",grey);
		fprintf(out,"N");
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		
		
		fprintf(out,"<tr>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Unmapped_0_canvas\" width=\"200\" height=\"100\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Unmapped_1_canvas\" width=\"200\" height=\"100\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Unmapped_2_canvas\" width=\"200\" height=\"100\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Unmapped_3_canvas\" width=\"200\" height=\"100\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Unmapped_4_canvas\" width=\"200\" height=\"100\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		fprintf(out,"</table>\n");
		
		fprintf(out,"</section>\n");
	}
}


void print_nucleotide_composition_script(FILE* out,struct seq_stats* seq_stats)
{
	
	int max_len[6];
	int min_len[6];
	int i,j,c;
	float sum = 0.0f;
	float add = 0.0f;
	float max = -1.0f;
	
	for(c= 0;c < 6;c++){
		
		max_len[c] = -1;
		min_len[c] =  MAX_SEQ_LEN+10;
		
		for(i = 0; i < MAX_SEQ_LEN;i++){
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len[c] ){
					max_len[c] = i;
				}
				
				if(i < min_len[c]){
					min_len[c] =  i;
				}
			}		
		}
		
		if(min_len[c] ==  MAX_SEQ_LEN+10){
			min_len[c] = 0;
		}
		
		if(max_len[c] == -1){
			max_len[c] = 0;
		}
	}	

	for(c= 0;c < 5;c++){
		if(seq_stats->alignments[c]){
			max = -1.0;
			for(j = 0; j < 5;j++){

				for(i = 0; i < max_len[c];i++){
					if(max <  seq_stats->nuc_composition[c][i][j]){
						max = seq_stats->nuc_composition[c][i][j];
					}
				}
			}
			sum = 1000000000;
			for(i = 0; i < 10;i++){
				if(max > sum){
					sum *= 10;
					break;
				}
				sum /= 10;
			}
			
			add = sum / 10;
			for(i = 0; i < 10;i++){
				if(max> sum){
					sum += add;
					break;
				}
				sum -= add;
			}
			
			
			for(j = 0; j < 5;j++){
				fprintf(out,"var graphCanvas = document.getElementById('Q%d0_%d_canvas');\n",c,j);
				fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
				fprintf(out,"var context = graphCanvas.getContext('2d');\n");
				fprintf(out,"var data = new Array(%d);\n",max_len[c]);
				for(i = 0; i < max_len[c];i++){
					fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1) ,(float) seq_stats->nuc_composition[c][i][j] / 1000.0f);
				}
				switch (j) {
					case 0:
						fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green,sum/ 1000.0f);
						break;
					case 1:
						fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",blue, sum/ 1000.0f);
						break;
						
					case 2:
						fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",yellow, sum/ 1000.0f);
						break;
						
					case 3:
						fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",red, sum/ 1000.0f);
						break;
						
					case 4:
						fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",grey, sum/ 1000.0f);
						break;
				}
				
				
				fprintf(out,"}\n");
				
			}
		}

	}
	if(seq_stats->alignments[5]){
		max = -1.0;
		for(j = 0; j < 5;j++){
			
			for(i = 0; i < max_len[5];i++){
				if(max <  seq_stats->nuc_composition[5][i][j]){
					max = seq_stats->nuc_composition[5][i][j];
				}
			}
		}
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		for(j = 0; j < 5;j++){
			fprintf(out,"var graphCanvas = document.getElementById('Unmapped_%d_canvas');\n",j);
			fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
			fprintf(out,"var context = graphCanvas.getContext('2d');\n");
			fprintf(out,"var data = new Array(%d);\n",max_len[5]);
			for(i = 0; i < max_len[5];i++){
				fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1), (float) seq_stats->nuc_composition[5][i][j]/ 1000.0f);
			}
			switch (j) {
				case 0:
					fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green,sum/ 1000.0f);
					break;
				case 1:
					fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",blue, sum/ 1000.0f);
					break;
					
				case 2:
					fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",yellow, sum/ 1000.0f);
					break;
					
				case 3:
					fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",red, sum/ 1000.0f);
					break;
					
				case 4:
					fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",grey, sum/ 1000.0f);
					break;
			}
			
			
			fprintf(out,"}\n");
			
		}
	}
	
	//	fprintf(out,"drawBarChart(context, data, 50, 100, (graphCanvas.height - 20), 50);\n");
	fprintf(out,"\n\n");
}


void print_quality_distribution_script(FILE* out,struct seq_stats* seq_stats,int solexa)
{
	int max_len = -1;
	int c,i;
	int min_len = MAX_SEQ_LEN+10;
	float total[6];
	for(c= 0;c < 6;c++){
		total[c] = 0.0f;
		for(i = 0; i < MAX_SEQ_LEN;i++){
			//total[c] += seq_stats->seq_len[c][i] ;
			if(seq_stats->seq_len[c][i] > total[c]){
				total[c] = seq_stats->seq_len[c][i] ;
			}
			
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len ){
					max_len = i;
				}
				
				if(i < min_len){
					min_len =  i;
				}
			}		
		}
	}	
	
	if(min_len ==  MAX_SEQ_LEN+10){
		min_len = 0;
	}
	
	if(max_len == -1){
		max_len = 0;
	}
	
	
	fprintf(out,"var graphCanvas = document.getElementById('quality_canvas');\n");
	fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
	fprintf(out,"var context = graphCanvas.getContext('2d');\n");
	fprintf(out,"var data = new Array(%d);\n",max_len- min_len);
	for(i = 0; i < max_len;i++){
		if(solexa){
			fprintf(out,"data[%d] = \"%d,",i,(i+1));
			for(c= 0;c < 5;c++){
				if(seq_stats->alignments[c]){
					//fprintf(stderr,"%f	%f\n",(float) seq_stats->seq_quality[c][i], (10.0f * log10(  pow(10.0f, (float) seq_stats->seq_quality[c][i] / 10.0f) +1.0f)));
					
					fprintf(out,"%0.2f,",(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-64*seq_stats->alignments[c]) /   (float)seq_stats->alignments[c]) / 10.0f) +1.0f)) );
					
					//fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1),(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-64*seq_stats->alignments[c]) /   (float)seq_stats->alignments[c]) / 10.0f) +1.0f)) );
				}else{
					fprintf(out,"0.0,");
				}
				
			}
			if(seq_stats->alignments[5]){
				fprintf(out,"%0.2f",(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[5][i]-64*seq_stats->alignments[5]) /   (float)seq_stats->alignments[5]) / 10.0f) +1.0f)) );
			}else{
				fprintf(out,"0.0");
			}
			
			fprintf(out,"\";\n");
			
		}else{
			
			fprintf(out,"data[%d] = \"%d,",i,(i+1));
			for(c= 0;c < 5;c++){
				if(seq_stats->alignments[c]){
					//fprintf(stderr,"%f	%f\n",(float) seq_stats->seq_quality[c][i], (10.0f * log10(  pow(10.0f, (float) seq_stats->seq_quality[c][i] / 10.0f) +1.0f)));
					
					fprintf(out,"%0.2f,",(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-33*seq_stats->alignments[c]) /   (float)seq_stats->alignments[c]) / 10.0f) +1.0f)) );
					
					//fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1),(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-64*seq_stats->alignments[c]) /   (float)seq_stats->alignments[c]) / 10.0f) +1.0f)) );
				}else{
					fprintf(out,"0.0,");
				}
				
			}
			if(seq_stats->alignments[5]){
				fprintf(out,"%0.2f",(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[5][i]-33*seq_stats->alignments[5]) /   (float)seq_stats->alignments[5]) / 10.0f) +1.0f)) );
			}else{
				fprintf(out,"0.0");
			}
			
			fprintf(out,"\";\n");
			
			//fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1), (float) (seq_stats->seq_quality[c][i]-33*seq_stats->alignments[c]) /   (float)seq_stats->alignments[c]);
		}
	}
			
	fprintf(out,"drawlineplot(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green,40.0f );
			
	fprintf(out,"}\n");
			
			
	
		
	
	/*if(seq_stats->alignments[5]){
		fprintf(out,"var graphCanvas = document.getElementById('Qun_quality_canvas');\n");
		fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
		fprintf(out,"var context = graphCanvas.getContext('2d');\n");
		fprintf(out,"var data = new Array(%d);\n",max_len- min_len);
		for(i = 0; i < max_len;i++){
			if(solexa){
				fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1),(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-64*seq_stats->alignments[c]) /   (float)seq_stats->alignments[5]) / 10.0f) +1.0f)) );
			}else{
				fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1),(float) (seq_stats->seq_quality[c][i]-33*seq_stats->alignments[c]) /   (float)seq_stats->alignments[5]);
			}
		}
		
		fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green, 40.0f);
		
		fprintf(out,"}\n");
		
		
	}
	
	fprintf(out,"\n\n");*/
}

void print_quality_distribution_canvas(FILE* out,struct seq_stats* seq_stats)
{
	//int i;	
	//for(i = 4;i >= 0;i--){ 
	//	if(seq_stats->alignments[i]){
			fprintf(out,"<section>\n");
			fprintf(out,"<h2>Mean Base Quality</h2>\n");
			fprintf(out,"<table>\n");
			fprintf(out,"<tr>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"<canvas id=\"quality_canvas\" width=\"700\" height=\"250\"></canvas>\n");
			fprintf(out,"</td>\n");
			fprintf(out,"</tr>\n");
			fprintf(out,"</table>\n");
			fprintf(out,"</section>\n");
	//	}
	/*}
	if(seq_stats->alignments[5]){
		fprintf(out,"<section>\n");
		fprintf(out,"<h2>Base Quality Distribution %s</h2>\n",q_legend[5]);
		fprintf(out,"<table>\n");
		fprintf(out,"<tr>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Qun_quality_canvas\" width=\"500\" height=\"250\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		fprintf(out,"</table>\n");
		fprintf(out,"</section>\n");
	}*/
	fprintf(out,"<pre>\n");
	fprintf(out,"Mean base quality of reads with low and high mapping quality.\n");
	
	fprintf(out,"</pre>");
}


void print_posteriors_distribution_script(FILE* out,struct seq_stats* seq_stats)
{
	int max_len = -1;
	int c,i;
	int min_len = MAX_SEQ_LEN+10;
	float total[6];
	for(c= 0;c < 6;c++){
		total[c] = 0.0f;
		for(i = 0; i < MAX_SEQ_LEN;i++){
			//total[c] += seq_stats->seq_len[c][i] ;
			if(seq_stats->seq_len[c][i] > total[c]){
				total[c] = seq_stats->seq_len[c][i] ;
			}
			
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len ){
					max_len = i;
				}
				
				if(i < min_len){
					min_len =  i;
				}
			}		
		}
	}	
	
	if(min_len ==  MAX_SEQ_LEN+10){
		min_len = 0;
	}
	
	if(max_len == -1){
		max_len = 0;
	}
	
	
	fprintf(out,"var graphCanvas = document.getElementById('posteriors_canvas');\n");
	fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
	fprintf(out,"var context = graphCanvas.getContext('2d');\n");
	fprintf(out,"var data = new Array(%d);\n",max_len- min_len);
	for(i = 0; i < max_len;i++){
		
		fprintf(out,"data[%d] = \"%d,",i,(i+1));
		for(c= 0;c < 5;c++){
			if(seq_stats->alignments[c]){
				//fprintf(stderr,"%f	%f\n",(float) seq_stats->seq_quality[c][i], (10.0f * log10(  pow(10.0f, (float) seq_stats->seq_quality[c][i] / 10.0f) +1.0f)));
				
				fprintf(out,"%0.2f,",(float) seq_stats->posterior_mappability[c][i] /   (float)seq_stats->alignments[c]);
				
				//fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,(i+1),(10.0f * log10(  pow(10.0f, (float) ((seq_stats->seq_quality[c][i]-64*seq_stats->alignments[c]) /   (float)seq_stats->alignmentsc]) / 10.0f) +1.0f)) );
			}else{
				fprintf(out,"0.0,");
			}
				
		}
		if(seq_stats->alignments[5]){
			fprintf(out,"%0.2f,",(float) seq_stats->posterior_mappability[5][i] /   (float)seq_stats->alignments[5]);
		}else{
			fprintf(out,"0.0");
		}
			
		fprintf(out,"\";\n");
			
	}
	
	fprintf(out,"drawlineplot(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green,40.0f );
	
	fprintf(out,"}\n");
}


void print_posteriors_distribution_canvas(FILE* out,struct seq_stats* seq_stats)
{
	fprintf(out,"<section>\n");
	fprintf(out,"<h2>Mean posterior probabilities</h2>\n");
	fprintf(out,"<table>\n");
	fprintf(out,"<tr>\n");
	fprintf(out,"<td>\n");
	fprintf(out,"<canvas id=\"posteriors_canvas\" width=\"700\" height=\"250\"></canvas>\n");
	fprintf(out,"</td>\n");
	fprintf(out,"</tr>\n");
	fprintf(out,"</table>\n");
	fprintf(out,"</section>\n");
}




void print_length_distribution_script(FILE* out,struct seq_stats* seq_stats)
{
	float sum = 0.0f;
	float add = 0.0f;
	int max_len = -1;
	int c,i;
	int min_len = MAX_SEQ_LEN+10;
	float total[6];
	for(c= 0;c < 6;c++){
		total[c] = 0.0f;
		for(i = 0; i < MAX_SEQ_LEN;i++){
			//total[c] += seq_stats->seq_len[c][i] ;
			if(seq_stats->seq_len[c][i] > total[c]){
				total[c] = seq_stats->seq_len[c][i] ;
			}
			
			if(seq_stats->seq_len[c][i] != 0){
				if(i  > max_len ){
					max_len = i;
				}
				if(i < min_len){
					min_len =  i;
				}
			}		
		}
	}	
	
	if(min_len ==  MAX_SEQ_LEN+10){
		min_len = 0;
	}
	
	if(max_len == -1){
		max_len = 0;
	}
	
	
	for(c= 0;c < 5;c++){
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(total[c] > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(total[c] > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		
		if(seq_stats->alignments[c]){
			fprintf(out,"var graphCanvas = document.getElementById('Q%d0_length_canvas');\n",c);
			fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
			fprintf(out,"var context = graphCanvas.getContext('2d');\n");
			fprintf(out,"var data = new Array(%d);\n",max_len- min_len);
			for(i = min_len; i <= max_len;i++){
				fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i-min_len,(i),(float) seq_stats->seq_len[c][i] /1000.0f);
			}

			fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Length\");\n",green, sum/ 1000.0f );
				
			fprintf(out,"}\n");
				

		}
		
	}
	c = 5;
	sum = 1000000000;
	for(i = 0; i < 10;i++){
		if(total[c] > sum){
			sum *= 10;
			break;
		}
		sum /= 10;
	}
	
	add = sum / 10;
	for(i = 0; i < 10;i++){
		if(total[c] > sum){
			sum += add;
			break;
		}
		sum -= add;
	}
	if(seq_stats->alignments[5]){
		fprintf(out,"var graphCanvas = document.getElementById('Qun_length_canvas');\n");
		fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
		fprintf(out,"var context = graphCanvas.getContext('2d');\n");
		fprintf(out,"var data = new Array(%d);\n",max_len- min_len);
		for(i = min_len; i <= max_len;i++){
			fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i-min_len,(i+1), (float)seq_stats->seq_len[5][i]/1000.0f );
		}
		
		fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Position\");\n",green, sum/ 1000.0f);
		
		fprintf(out,"}\n");
		
		
	}
	
	fprintf(out,"\n\n");
}

void print_length_distribution_canvas(FILE* out,struct seq_stats* seq_stats)
{
	int i;	
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i]){
			fprintf(out,"<section>\n");
			fprintf(out,"<h2>Length Distribution %s</h2>\n",q_legend[i]);
			fprintf(out,"<table>\n");
			fprintf(out,"<tr>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"<canvas id=\"Q%d0_length_canvas\" width=\"500\" height=\"250\"></canvas>\n",i);
			fprintf(out,"</td>\n");
			fprintf(out,"</tr>\n");
			fprintf(out,"</table>\n");
			fprintf(out,"</section>\n");
		}
	}
	if(seq_stats->alignments[5]){
		fprintf(out,"<section>\n");
		fprintf(out,"<h2>Length Distribution %s</h2>\n",q_legend[5]);
		fprintf(out,"<table>\n");
		fprintf(out,"<tr>\n");
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Qun_length_canvas\" width=\"500\" height=\"250\"></canvas>\n");
		fprintf(out,"</td>\n");
		fprintf(out,"</tr>\n");
		fprintf(out,"</table>\n");
		fprintf(out,"</section>\n");
	}
}



void print_error_distribution_script(FILE* out,struct seq_stats* seq_stats)
{
	int i,c;
	float total[5];
	float sum = 0.0f;
	float add = 0.0f;
	for(i = 0; i < 5;i++){
		total[i] = 0.0f;
		for(c =0;c < MAXERROR;c++){
			
			
			//total[i] += seq_stats->errors[i][c];
			if(seq_stats->errors[i][c] > total[i] ){
				total[i] = seq_stats->errors[i][c];
			}
		}
	}
	//max = 10000.0;
	for(c= 0;c < 5;c++){
		
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(total[c] > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(total[c] > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		
		
		if(seq_stats->alignments[c]){
			fprintf(out,"var graphCanvas = document.getElementById('Q%d0_error_canvas');\n",c);
			fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
			fprintf(out,"var context = graphCanvas.getContext('2d');\n");
			fprintf(out,"var data = new Array(%d);\n",MAXERROR);
			for(i = 0; i < MAXERROR;i++){
				fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,i,(float) seq_stats->errors[c][i] /1000.0f);
			}
			
			fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"Errors\");\n",green,  sum/ 1000.0f);
			
			fprintf(out,"}\n");
		}
	}
	fprintf(out,"\n\n");
}



void print_error_distribution_canvas(FILE* out,struct seq_stats* seq_stats)
{
	int i;	
	
	fprintf(out,"<section>\n");
	
	fprintf(out,"<h2>Error Distribution</h2>\n");
	fprintf(out,"<table>\n");
	
	fprintf(out,"<tr style = \"text-align:center; font:  bold 12pt Arial;\">\n");
	for(i = 4;i >= 0;i--){ 
		fprintf(out,"<td>\n");
		fprintf(out,"%s",q_legend[i]);
		fprintf(out,"</td>\n");
	}
	fprintf(out,"</tr>\n");
	
	
	fprintf(out,"<tr>\n");
	for(i = 4;i >= 0;i--){ 
		fprintf(out,"<td>\n");
		fprintf(out,"<canvas id=\"Q%d0_error_canvas\" width=\"200\" height=\"200\"></canvas>\n",i);
		fprintf(out,"</td>\n");
	}
	fprintf(out,"</tr>\n");
	fprintf(out,"</table>\n");
	
	fprintf(out,"</section>\n");

}



void print_mapping_stats_canvas(FILE* out,struct seq_stats* seq_stats)
{
	float mapped,total;
	int i;
	
	mapped = 0.0f;
	total = 0.0f;
	
	for(i= 0;i < 5;i++){
		mapped += seq_stats->alignments[i];
	}
	total = mapped + seq_stats->alignments[5];
	
	fprintf(out,"<section>\n");
	
	
	fprintf(out,"<h2>Mapping stats: %0.0f%% aligned (%0.1fM aligned out of %0.1fM total) </h2>\n", mapped / total *100.0f,mapped / 1000000.0f, total / 1000000.0f );
        
	
	fprintf(out,"<canvas id=\"mapping_stats_canvas\" width=\"700\" height=\"250\"></canvas>\n");
	fprintf(out,"<pre>\n");
	fprintf(out,"Number of alignments in various mapping quality (MAPQ) intervals and number of unmapped sequences. The percentage and number of alignments in each category is given in brackets.\n");
	
	fprintf(out,"</pre>");
	
	fprintf(out,"</section>\n");
}

void print_mapping_stats_script(FILE* out,struct seq_stats* seq_stats)
{
	int i;
	float old_start = 0;
	float areas[6];
	float total = 0.0f;
	for(i= 0;i < 6;i++){
		areas[i] = seq_stats->alignments[i];
		total += seq_stats->alignments[i];
	}
	for(i= 0;i < 6;i++){
		areas[i] = (areas[i] / total  * 360);
		//fprintf(stderr,"%d	%f	%d\n",i,areas[i] ,seq_stats->alignments[i]);
	}
	
	fprintf(out,"var canvas = document.getElementById(\"mapping_stats_canvas\");\n");
	fprintf(out,"if (canvas.getContext) {\n");
	fprintf(out,"var ctx = canvas.getContext(\"2d\");\n");
	
	for(i= 4;i >=0;i--){
		if(seq_stats->alignments[i]){
		switch (i) {
			case 0:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",blue);
				break;
			case 1:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",green);
				break;
			case 2:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",lemon);
				break;
			case 3:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",orange);
				break;
			case 4:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",red);
				break;
		}
		fprintf(out,"ctx.beginPath();\n");
		
		if(areas[i]+old_start >= 360){
			fprintf(out,"ctx.arc(120,120,100,Math.PI/180 *%f,Math.PI/180 * %f,false); \n",old_start, 360.0f);
		}else{
		
			fprintf(out,"ctx.arc(120,120,100,Math.PI/180 *%f,Math.PI/180 * %f,false); \n",old_start, areas[i]+old_start);
		}
		fprintf(out,"ctx.lineTo(120,120);\n");  
		fprintf(out,"ctx.fill();\n");
		old_start =  areas[i] + old_start;
		}
	}
	if(seq_stats->alignments[5]){
		fprintf(out,"ctx.fillStyle = \"#%x\"\n",grey);
		fprintf(out,"ctx.beginPath();\n");
		if(old_start >= 360){
			fprintf(out,"ctx.arc(120,120,100,Math.PI/180 *%f,Math.PI/180 * %f,false); \n",360.0f,360.0f);
		}else{
			fprintf(out,"ctx.arc(120,120,100,Math.PI/180 *%f,Math.PI/180 * %f,false); \n",old_start,360.0f);
		}
		fprintf(out,"ctx.lineTo(120,120);\n");  
		fprintf(out,"ctx.fill();\n");
	}
	
	//legend
	old_start =10;
	fprintf(out,"ctx.font = \"12pt Arial\";\n");
	for(i= 4;i >=0;i--){
		switch (i) {
			case 0:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",blue);
				break;
			case 1:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",green);
				break;
			case 2:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",lemon);
				break;
			case 3:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",orange);
				break;
			case 4:
				fprintf(out,"ctx.fillStyle = \"#%06x\"\n",red);
				break;
		}
	
		fprintf(out,"ctx.fillRect (240, %f, 20, 20);",old_start); 
		fprintf(out,"ctx.fillStyle = \"#%06x\"\n",black);
		fprintf(out,"ctx.fillText(\"%s (%2.1f%% , %d)\", 270, %f);\n",q_legend2[i], (float)seq_stats->alignments[i] / total *100.0f,seq_stats->alignments[i], old_start + 15);
		
		
		
		old_start += 35;
		
	}
	fprintf(out,"ctx.fillStyle = \"#%x\"\n",grey);
	fprintf(out,"ctx.fillRect (240, %f, 20, 20);",old_start); 
	fprintf(out,"ctx.fillStyle = \"#%06x\"\n",black);
	fprintf(out,"ctx.fillText(\"Unmapped (%2.1f%% , %d)\", 270, %f);\n", (float)seq_stats->alignments[5] / total *100.0f,seq_stats->alignments[5], old_start + 15);
	
	
	fprintf(out,"}\n");
	fprintf(out,"\n\n");
}





void print_error_profile_canvas(FILE* out,struct seq_stats* seq_stats)
{
	int i;
	for(i = 4;i >= 0;i--){ 
		if(seq_stats->alignments[i]){
			fprintf(out,"<section>\n");
			
			fprintf(out,"<h2>Error Profile %s</h2>\n",q_legend[i]);
			
			fprintf(out,"<table>\n");
			
			fprintf(out,"<tr style = \"text-align:center; font:  bold 12pt Arial;\">\n");
			fprintf(out,"<td>\n");
			fprintf(out,"Mismatches");
			fprintf(out,"</td>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"Insertions");
			fprintf(out,"</td>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"Deletions");
			fprintf(out,"</td>\n");
			fprintf(out,"</tr>\n");
			fprintf(out,"<tr>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"<canvas id=\"Error_profile_mismatch%d\" width=\"300\" height=\"200\"></canvas>\n",i);
			fprintf(out,"</td>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"<canvas id=\"Error_profile_insertions%d\" width=\"300\" height=\"200\"></canvas>\n",i);
			fprintf(out,"</td>\n");
			fprintf(out,"<td>\n");
			fprintf(out,"<canvas id=\"Error_profile_deletions%d\" width=\"300\" height=\"200\"></canvas>\n",i);
			fprintf(out,"</td>\n");
			fprintf(out,"</tr>\n");
			fprintf(out,"</table>\n");
			fprintf(out,"</section>\n");
		}
	}
}



void print_error_profile_script(FILE* out,struct seq_stats* seq_stats)
{
	int i,j,f;
	float sum = 0.0f;
	float max = -1.0f;
	float add = 0.0f;
	
	for(f = 4;f >= 0;f--){
		fprintf(out,"var graphCanvas = document.getElementById('Error_profile_mismatch%d');\n",f);
		fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
		fprintf(out,"var context = graphCanvas.getContext('2d');\n");
		fprintf(out,"var data = new Array(%d);\n",seq_stats->max_len);
		for(i = 0; i < seq_stats->max_len;i++){
			fprintf(out,"data[%d] = new Array(4)\n",i);
			sum = 0;
			for(j = 0; j < 4;j++){
				sum += seq_stats->mismatches[f][i][j];
			}
			
			for(j = 0; j < 4;j++){
				fprintf(out,"data[%d][%d] = \"%0.2f\";\n",i,j, (float) seq_stats->mismatches[f][i][j]/1000.f);
			}
			if(sum > max){
				max = sum;
			}
		}
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		
		//fprintf(stderr, "%f	%f\n",sum,max);
		fprintf(out,"drawBarChart2(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),%f);\n", sum/ 1000.0f);
		
		fprintf(out,"}\n");
		
		max = -1.0f;
		fprintf(out,"var graphCanvas = document.getElementById('Error_profile_insertions%d');\n",f);
		fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
		fprintf(out,"var context = graphCanvas.getContext('2d');\n");
		fprintf(out,"var data = new Array(%d);\n",seq_stats->max_len);
		for(i = 0; i < seq_stats->max_len;i++){
			fprintf(out,"data[%d] = new Array(4)\n",i);
			sum = 0;
			for(j = 0; j < 4;j++){
				sum += seq_stats->insertions[f][i][j];
			}
			
			for(j = 0; j < 4;j++){
				fprintf(out,"data[%d][%d] = \"%0.2f\";\n",i,j, (float) seq_stats->insertions[f][i+1][j] / 1000.0f);
			}
			if(sum > max){
				max = sum;
			}
		}
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		
		//fprintf(stderr, "%f	%f\n",sum,max);
		fprintf(out,"drawBarChart2(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),%f);\n", sum/ 1000.0f);
		
		fprintf(out,"}\n");
		
		
		max = -1.0f;
		fprintf(out,"var graphCanvas = document.getElementById('Error_profile_deletions%d');\n",f);
		fprintf(out,"if (graphCanvas && graphCanvas.getContext) {\n");
		fprintf(out,"var context = graphCanvas.getContext('2d');\n");
		fprintf(out,"var data = new Array(%d);\n",seq_stats->max_len);
		for(i = 0; i < seq_stats->max_len;i++){
			fprintf(out,"data[%d] = \"%d,%0.2f\";\n",i,i+1, (float)seq_stats->deletions[f][i]/1000.0f);
			sum = seq_stats->deletions[f][i];
			if(sum > max){
				max = sum;
			}
		}
		sum = 1000000000;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum *= 10;
				break;
			}
			sum /= 10;
		}
		
		add = sum / 10;
		for(i = 0; i < 10;i++){
			if(max > sum){
				sum += add;
				break;
			}
			sum -= add;
		}
		fprintf(out,"drawBarChart(context, data, 0, (graphCanvas.width - 10), (graphCanvas.height - 10),\"#%06x\",%f,\"position\");\n",grey,  sum/ 1000.0f);
		//fprintf(stderr, "%f	%f\n",sum,max);
		
		
		fprintf(out,"}\n");
	}
	
}



