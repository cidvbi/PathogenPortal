#!/usr/bin/env python
# -*- coding: utf-8 -*-

#expects a folder structure gtf/gtf_file fasta/fasta.fa
#to run: find ./eupathdb -type f -name "*.fasta" -exec python pprnaseq_adjust_gtf.py {} eupathdb \;
import os, sys, fileinput


## return key and value for storage
def process_patric_header(line):
    contents=line.strip()[1:] #remove > and \n
    cur_value=contents.split()[0]
    cur_key=cur_value.split('|')[-1]
    return [(cur_key, cur_value)], line

##return modified gtf line for patric
def mod_patric(id_lookup, line):
    pieces=[]
    cur_key=""
    if(line.startswith('#')):
        if(line.startswith("# sequence-region")):
              columns=line.split()
              if len(columns) >=3:
                  cur_key=to_replace=columns[2]
        else:
            return line
    else:
        columns=line.split("\t")
        cur_key=to_replace=columns[0]
    if(cur_key in id_lookup):
        replacement=id_lookup[cur_key]
        return (line.replace(to_replace, replacement))
    return(line)

##This function also modifies the line in-place using fileinput
def process_vector_header(line):
    results=[]
    contents=line.strip()[1:] #remove > and \n
    cur_key=contents.split()[0]
    cur_value=contents.split()[0].replace(':','|') # get modified value for replacement
    #get rid of colons that mess up cufflinks IN THE FASTA FILE
    line=line.replace(cur_key,cur_value)
    results.append((cur_key, cur_value))
    pieces=cur_key.split(':')
    if len(pieces)>=3:
        cur_key=pieces[2]
        results.append((cur_key,cur_value))
    return results, line

def mod_vector(id_lookup, line):
    pieces=[]
    cur_key="NOTRETRIEVED"
    if(line.startswith('#')):
        if(line.startswith("##sequence-region")):
              columns=line.split()
              if len(columns) >=2:
                  cur_key=columns[1]
                  to_replace=columns[1]
        else:
            return line
    else:
        columns=line.split("\t")
        cur_key=columns[0]
        to_replace=columns[0]
    if(cur_key in id_lookup):
        replacement=id_lookup[cur_key]
        return (line.replace(to_replace,replacement))
    return(line)    
    
def process_eupath_header(line):
    contents=line.strip()[1:] #remove > and \n
    cur_value=contents.split()[0]
    pieces=cur_value.split('|')
    if(len(pieces)>=2):
        cur_key=pieces[1]
    else:
        #write to stderr here
        cur_key="NOTFOUND"
    return [(cur_key, cur_value)], line
    
##return modified gtf line for eupath
def mod_eupath(id_lookup, line):
    pieces=[]
    if(line.startswith('#')):
        if(line.startswith("##sequence-region")):
              columns=line.split("\t")
              if len(columns) >=2:
                  pieces=columns[1].split('|')
                  to_replace=columns[1]
        else:
            return line
    else:
        columns=line.split("\t")
        pieces=columns[0].split('|')
        to_replace=columns[0]
    if len(pieces)>=2:
        cur_key=pieces[1]
    else:
        cur_key="NOTRETRIEVED"
    if(cur_key in id_lookup):
        replacement=id_lookup[cur_key]
        return (line.replace(to_replace,replacement))
    return(line)

    
## Returns the command to process/parse the header in the fasta file and the command to modify lines in the GTF file to match them
## Also calls a prepocess function on the fasta file
def set_mode(brc_mode):
    if(brc_mode == 'patric'):
        return (process_patric_header, mod_patric, 0)
    elif(brc_mode == 'eupathdb'):
        return (process_eupath_header, mod_eupath, 0)
    elif(brc_mode == 'vectorbase'):
        return (process_vector_header, mod_vector, 1)
    else:
        assert False, "no valid brc mode"



def main(init_args):
    if(len(init_args)<2):
        print "Usage: pprnaseq_adjust_gtf.py  <fasta file> <brc mode>"
        exit(0)
    id_lookup={}
    my_file=init_args[0]
    brc_mode=init_args[1]
    process_header, process_result, mod_mode = set_mode(brc_mode)
    my_path, my_file_base = os.path.split(my_file)
    parent_path, my_folder = os.path.split(my_path)
    for line in fileinput.input(my_file, inplace=mod_mode):
        if line.startswith('>'):
            results, line=process_header(line)
            for cur_key, cur_value in results:
                id_lookup[cur_key]=cur_value
        if(mod_mode):
            sys.stdout.write(line)
    gtf_folder = os.path.join(parent_path,'gtf')
    gtfList=os.listdir(gtf_folder)
    for gtf_file in gtfList:
        cur_file=os.path.join(gtf_folder, gtf_file)
        os.rename(cur_file, cur_file+".orig")
        gtf_out=open(cur_file, 'w')
        for line in open(cur_file+".orig"):
            gtf_out.write(process_result(id_lookup, line))
        gtf_out.close()

if __name__ == "__main__":
        main(sys.argv[1:])