VCF2FAS:  Converts VCF file(s) to fasta format.  
  
Author: B. Nevado  
  
Usage:  
vcf2fas -reference reference.fas -vcfs samples.txt  
    -reference: reference genome/transcriptome used.  
    -vcfs: text file with path to vcf file(s) to use.  
  
Output: 1 fasta file per contig named 'contig_name.fas' to current folder.  
  
Requirements:  
    BIO++ (bpp-core and bpp-seq libraries)  
    VCF files must be obtained with SAMtools v1.0+, and filtered with bcftools (should contain 'PASS' for confident variants).  
    Each vcf file must contain a single individual.  
    Reference homozygous blocks can be present.  
  
Notes:  
    INDELS are ignored.  
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
  
