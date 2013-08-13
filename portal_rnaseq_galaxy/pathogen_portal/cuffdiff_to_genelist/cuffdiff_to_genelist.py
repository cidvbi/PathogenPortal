#!/usr/bin/env python
# -*- coding: utf-8 -*-

#expects a folder structure gtf/gtf_file fasta/fasta.fa
#to run: find ./eupathdb -type f -name "*.fasta" -exec python pprnaseq_adjust_gtf.py {} eupathdb \;
import os, sys
from math import log


def main(init_args):
    if(len(init_args)<1):
        print "Usage cuffdiff_to_genelist.py  <cuffdiff files>"
        exit(0)
    for input_file in init_args:
        input_handle=open(input_file, 'r')
        lines=input_handle.readlines()
        for line in lines[1:]:
            parts=line.strip().split('\t')
            status='NOT OK'
            if len(parts)==14:
                try:
                    (gene_id, sample1, sample2, status, value1, value2, log_change) = (parts[1], parts[4], parts[5], parts[6], float(parts[7]), float(parts[8]), float(parts[9]))
                except ValueError:
                    sys.stderr.write('One of the input files does not match the formatting of a CuffDiff gene differential expression testing file\n')
                    sys.exit()
            else:
                sys.stderr.write('One of the input files does not match the formatting of a CuffDiff gene differential expression testing file\n')
                sys.exit()
            if status != 'OK':
                continue
            changed=False
            if value1 == 0:
                value1=0.01
                changed=True
            if value2 == 0:
                value2= 0.01
                changed=True
            if changed:
                log_change= log(value2/value1)/log(2)
            print '\t'.join([gene_id, sample1+'|'+sample2, str(log_change)])
        input_handle.close()

if __name__ == "__main__":
        main(sys.argv[1:])
