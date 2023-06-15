VCF2FAS:  Converts VCF file(s) to fasta format.  
  
Author: B. Nevado  
  
Usage: vcf2fas -reference reference.fas -vcfs samples.txt
-reference: reference genome/transcriptome used.
-vcfs: text file with path to vcf files to use.
-gf: which field to use for genotypes: GT or PL.
-strictIUPAC: if set to 0, errors in assigning IUPAC codes will not crash program (maybe useful for debugging?).


Ouput: 1 fasta file per contig named 'contig_name.fas' to current folder.


Requirements: BIO++ (bio-core and bio-seq v 2.4.0)
              VCF files must be obtained with SAMtools v1.0+, and filtered with bcftools (should contain 'PASS' for confident variants).
              Reference homozygous calls can be present.


Notes: 	INDELS are ignored.
       	Heterozygous genotypes are coded with IUPAC.
		Only tested with vcf files obtained with SAMtools v1+.  
  
Installation (Linux):  
git clone https://github.com/brunonevado/vcf2fas.git  
cd vcf2fas  
make  
./vcf2fas  

Installation (Linux / Mac):  
If bpp-core and bpp-seq libraries are installed and available on PATH, this should work (from within /source):  
 g++ -O3 -std=c++11 *cpp -lbpp-core -lbpp-seq -o vcf2fas  
  
