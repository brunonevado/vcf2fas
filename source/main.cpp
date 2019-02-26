/*
 Convert VCF output from SAMtools v1+ into fasta
 Input VCFs should contain a single individual, and be filtered (PASS field must be present)
 Needs reference sequence (-reference) and a text file with path to vcf files to use (-infiles).
 Optionally can take a list of contigs to use (-contigs) – default is all contigs. Option not implemented yet.
 Output files (1 per contig) will be named contig_name.fas.
 
 Uses BIO++
 
 */

#include <iostream>

#include <Bpp/Seq/Alphabet.all> /* this includes all alphabets in one shot */
#include <Bpp/Seq/Container.all> /* this includes all containers */
#include <Bpp/Seq/Io.all> /* this includes all sequence readers and writers */
#include <Bpp/App/ApplicationTools.h>
#include <Bpp/Seq/SiteTools.h>

#include "args.h"
#include "vcf.h"

// 14072015: throwing error without catch fixed
// 24042015: trying to fix linux-only bug
// 2602019: added option to use genotype calls directly from GT field or from PL values

void help(){
    std::cout << "###################\n  vcf2fas 26022019 \n###################" << std::endl;;
    std::cout << "Create fasta files from vcf files." << std::endl;;
    std::cout << "Usage: vcf2fas -reference reference.fas -vcfs samples.txt" << std::endl;
    std::cout << "-reference: reference genome/transcriptome used." << std::endl;
    std::cout << "-vcfs: text file with path to vcf files to use." << std::endl;
    std::cout << "-gf: which field to use for genotypes: GT or PL." << std::endl;
   std::cout << "Ouput: 1 fasta file per contig named 'contig_name.fas' to current folder." << std::endl;
    
    std::cout << "Requirements: BIO++" << std::endl;
    std::cout << "              VCF files must be obtained with SAMtools v1.0+, and filtered with bcftools (should contain 'PASS' for confident variants)." << std::endl;
    std::cout << "              Reference homozygous calls can be present." << std::endl;
    std::cout << "Notes: INDELS are ignored." << std::endl;
    std::cout << "       Heterozygous genotypes are coded with IUPAC." << std::endl;
    
    
}


int main(int argc, const char * argv[]) {
    
    bool outputFasta = true;
    
    sargs myargs;
    try{
        myargs = args::getargs(argc, argv, std::vector<std::string> {"reference","vcfs","gf"}, std::vector<std::string> {}, std::vector<std::string>  {}, std::string {"contigs"}, std::string {}); }
    catch (std::string e){
        std::cout << " Args failed: " << e << std::endl;
        help();
        exit(1);
    }
    
    std::string reference = myargs.args_string.at(0);
    std::string infilesVCF = myargs.args_string.at(1);
    std::string genotypeField = myargs.args_string.at(2);
    std::string infileCONTIGS = (myargs.args_string_optional.size() > 0) ? myargs.args_string_optional.at(0) : "";
    
    if( genotypeField != "GT" && genotypeField != "PL" ){
        std::cerr << "ERROR: Genotype field (-gf) must be GT or PL" << std::endl;
        exit(1);
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
        vcf avcf(cfile);
        
        try {
            avcf.readfile(genotypeField);
        }
        catch(std::string e){
            std::cerr << "ERROR READING VCF FILE " << cfile << " : " <<  e << std::endl;
            exit(1);
        }
        vec_vcfs.push_back(avcf);
        std::clog << "Finished parsing infile " << cfile << ", ambiguous/total lines: " << avcf.get_n_failed_lines() << "/" << avcf.get_total_variants_read() << std::endl;
    }
    
    // READ REF
    bpp::Fasta fasReader(-1);
    fasReader.strictNames(true);
    bpp::OrderedSequenceContainer *reference_genome = fasReader.readSequences(reference, &bpp::AlphabetTools::DNA_ALPHABET);
    std::clog << "Read " << reference_genome->getNumberOfSequences() << " contigs form reference file " << reference << std::endl;
    
    if( infileCONTIGS == "" ){
        for ( unsigned int icontig = 0 ; icontig < reference_genome->getNumberOfSequences() ; icontig++ ) {
            bpp::AlignedSequenceContainer bpp_contig ( &bpp::AlphabetTools::DNA_ALPHABET );
            
            try{
                for (unsigned int iind = 0; iind < vec_vcfs.size(); iind++) {
                    std::string temp = vec_vcfs.at(iind).make_fas(reference_genome->getSequencesNames().at(icontig), reference_genome->getSequence(icontig).toString());
                    bpp::Sequence *sequence = new bpp::BasicSequence( vec_vcfs.at(iind).get_ind_name() , temp, &bpp::AlphabetTools::DNA_ALPHABET);
                    bpp_contig.addSequence(*sequence);
                    delete  sequence;
                }
            }
            catch(...){
                std::cerr << "ERROR: problems doing contig " << reference_genome->getSequencesNames().at(icontig) << std::endl;
                exit(1);
            }
            
            if( outputFasta == true ){
                try{
                    fasReader.writeSequences(std::string( reference_genome->getSequencesNames().at(icontig) + ".fas"  ), bpp_contig );
                }
                catch(...){
                    std::cerr << "ERROR: problems writing to file contig " << reference_genome->getSequencesNames().at(icontig) << std::endl;
                    exit(1);
                }
            }
            else{
                
                bpp::SiteContainer *vsc2 = new bpp::VectorSiteContainer(bpp_contig);
                
                for( unsigned int isite = 0; isite < vsc2->getNumberOfSites() ; isite++ ){
                    
                    if (bpp::SiteTools::getNumberOfDistinctCharacters(vsc2->getSite(isite)) <=2){continue;}
                    
                    std::cout << reference_genome->getSequencesNames().at(icontig) << "\t" << vsc2->getSite(isite).getPosition() << "\t"
                    << vsc2->getSite(isite).toString() << "\t" << bpp::SiteTools::getNumberOfDistinctCharacters(vsc2->getSite(isite))
                    
                    << std::endl;
                }
            }
            
            if( icontig % 100 == 0 ){
                bpp::ApplicationTools::displayGauge(icontig/100, (reference_genome->getNumberOfSequences()/100)-1, '.' );
            }
        }
    }
    else{
        std::cerr << "Contigs option not available yet!" << std::endl;
        exit(1);
    }
    
    std::clog << std::endl << "Finished writing fasta files." << std::endl;
    return 0;
}
