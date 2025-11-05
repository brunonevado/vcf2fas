###################  
  vcf2fas v2.051125  
###################  
  
Create fasta files from vcf files.  
Usage: vcf2fas -reference reference.fas -vcfs samples.txt  
-reference: reference genome/transcriptome used.  
-vcfs: text file with path to vcf files to use.  
-gf: which field to use for genotypes: GT or PL.  
-strictIUPAC: if set to 0, errors in assigning IUPAC codes will not crash program (maybe useful for debugging? - default is true).  
Ouput: 1 fasta file per contig named 'contig_name.fas' to current folder.  
Requirements: VCF files must be obtained with SAMtools v1.0+, and filtered with bcftools (should contain 'PASS' for confident variants).  Reference homozygous calls can be present.  
Notes: INDELS are ignored.  
       Heterozygous genotypes are coded with IUPAC.  



INSTALLATION:  
  
git clone https://github.com/brunonevado/vcf2fas  
cd vcf2fas/  
rm -r build/  
mkdir build  
cd build  
cmake ../  
make  

