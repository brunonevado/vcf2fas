/*
 Convert VCF output from SAMtools v1+ into fasta
 Input VCFs should contain a single individual, and be filtered (PASS field must be present)
 Needs reference sequence (-reference) and a text file with path to vcf files to use (-vcfs).
 Output files (1 per contig) will be named contig_name.fas.

 */

#include <iostream>
#include <unordered_map>

#include "args2.h"
#include "vcf.h"
# include "fasta.h"

// 14072015: throwing error without catch fixed
// 24042015: trying to fix linux-only bug
// 2602019: added option to use genotype calls directly from GT field or from PL values
// 21052021: changed vcf.cpp to detect indels based on regex
// 21052021b: added strictIUPAC option - to ignore (with warning) errors turning alleles to iupac codes
// 150623: added ability to read 1-bp homozygous blocks with MinDP instead of END flag
// 260624: added contig option


std::string Pversion = "v2.260624";

void help(){
    std::cout << "###################\n  vcf2fas "<< Pversion << "\n###################" << std::endl;;
    std::cout << "Create fasta files from vcf files." << std::endl;;
    std::cout << "Usage: vcf2fas -reference reference.fas -vcfs samples.txt" << std::endl;
    std::cout << "-reference: reference genome/transcriptome used." << std::endl;
    std::cout << "-vcfs: text file with path to vcf files to use." << std::endl;
    std::cout << "-gf: which field to use for genotypes: GT or PL." << std::endl;
    std::cout << "-strictIUPAC: if set to 0, errors in assigning IUPAC codes will not crash program (maybe useful for debugging? - default is true)." << std::endl;
    std::cout << "Ouput: 1 fasta file per contig named 'contig_name.fas' to current folder." << std::endl;
    
    std::cout << "Requirements: VCF files must be obtained with SAMtools v1.0+, and filtered with bcftools (should contain 'PASS' for confident variants)." << std::endl;
    std::cout << "              Reference homozygous calls can be present." << std::endl;
    std::cout << "Notes: INDELS are ignored." << std::endl;
    std::cout << "       Heterozygous genotypes are coded with IUPAC." << std::endl;
    
    
}


int main(int argc, const char * argv[]) {
     
    // read command line options
    margs programOptions;
    try{
        programOptions.getargs(argc, argv, std::vector <std::string> {
            "reference,s,f",
            "vcfs,s,f",
            "gf,s,t",
            "strictIUPAC,b,t",
            "contigs,s,t",
            "verbose,b,t"
            });
    }catch(std::string e){ help();std::cerr << std::endl << "Failed reading args: " << e << std::endl;exit(1);}

    std::string reference =programOptions.getString("reference");
    std::string infilesVCF =programOptions.getString("vcfs");
    std::string genotypeField = programOptions.isArgDefined("gf") ? programOptions.getString("gf") : "GT";
    std::string infileContigs = programOptions.isArgDefined("contigs") ? programOptions.getString("contigs") : "";
    bool strictIUPAC = programOptions.isArgDefined("strictIUPAC") ? programOptions.getBool("strictIUPAC") : true;
    bool verbose = programOptions.isArgDefined("verbose") ? programOptions.getBool("verbose") : false;

    if( genotypeField != "GT" && genotypeField != "PL" ){
        std::cerr << "ERROR: Genotype field (-gf) must be GT or PL" << std::endl;
        exit(1);
    }

    // READ CONTIG NAMES (IF DEFINED)
    std::unordered_map < std::string, bool > contigNames;
    if(infileContigs != ""){
        std::ifstream fh_contigs ( infileContigs );
        if( !fh_contigs.is_open() ){
            std::cerr << "ERROR: Unable to open for reading contigs infile " << infileContigs << std::endl;
            exit(1);
        }
        std::string cfile;
        while (getline(fh_contigs, cfile)) {
            if(cfile.length() > 0){
                contigNames[cfile] = true;
            }
        }
        if(verbose){
            std::cout << "Read " << contigNames.size() << " contig names from file " << infileContigs << std::endl; 
        }
    }

    // READ VCFS
    std::vector < vcf > vec_vcfs;
    std::ifstream fh_vcfs ( infilesVCF );
    if( !fh_vcfs.is_open() ){
        std::cerr << "ERROR: Unable to open for reading infile " << infilesVCF << std::endl;
        exit(1);
    }
    std::string cfile;
    while (getline(fh_vcfs, cfile)) {
        vcf avcf(cfile, strictIUPAC);
        avcf.set_contigs(contigNames);

        try {
            avcf.readfile(genotypeField);
        }
        catch(std::string e){
            std::cerr << "ERROR READING VCF FILE " << cfile << " : " <<  e << std::endl;
            exit(1);
        }
        vec_vcfs.push_back( avcf);
        std::clog << "Finished parsing infile " << cfile << ", ambiguous/total lines: " << avcf.get_n_failed_lines() << "/" << avcf.get_total_variants_read() << std::endl;
    }

    // READ REF
    fasta genomeReferenceFasta(Pversion);
    try{
        genomeReferenceFasta.readFastaFile(reference, false, verbose);
        for ( unsigned int icontig = 0 ; icontig < genomeReferenceFasta.getNumberOfSequences() ; icontig++ ) {
            if(contigNames.size() != 0 && contigNames.count(genomeReferenceFasta.getSequenceName(icontig)) == 0 ){continue;}
            fasta currentContig(Pversion);
            for (unsigned int iind = 0; iind < vec_vcfs.size(); iind++) {
              currentContig.addSequence(vec_vcfs.at(iind).get_ind_name(), vec_vcfs.at(iind).make_fas(genomeReferenceFasta.getSequenceName(icontig), genomeReferenceFasta.getSequence(icontig)), (iind == 0) ? false : true, false );
            }
            currentContig.writeFastaFile( std::string( genomeReferenceFasta.getSequenceName(icontig) + ".fas" ), verbose );
        }
    }
    catch(std::string e){
        std::cerr << "ERROR: problems writing fasta file: " << e << std::endl;
        exit(1);
    }

    std::clog << std::endl << "Finished writing fasta files." << std::endl;
    return 0;
}
