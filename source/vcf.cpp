//
//  vcf.cpp
//  vcf2fas
//
//  Created by bnevado on 03/10/2014.
//

#include "vcf.h"


vcf::vcf ( std::string in, bool iupacStrictness ){
    infile = in;
    failed_parse = 0;
    iupacStrict = iupacStrictness;
}

void vcf::readfile ( std::string gfField) {
    std::ifstream fh_infile;
    std::string cline;
    
    fh_infile.open(infile);

    if(!fh_infile.is_open()){  throw std::string ( "Unable to open infile vcf ");}
    
    
    while( getline(fh_infile, cline) ){
        if( strncmp(cline.c_str(), "##", 2) == 0){
            continue;
        }
        else if( strncmp(cline.c_str(), "#CHR", 3) == 0){
            std::vector <std::string > fields = msplit(cline, "\t");
            if( fields.size() != 10 ){ throw std::string("more than 1 individual in infile vcf?");  }
            ind_name = fields.at(9);
            continue;
        }
        else{
            std::vector <std::string > fields = msplit(cline, "\t");
            vcf_line aline;
            try{
               // if ( fields.at(6) != "PASS" || fields.at(7).substr(0,5) == "INDEL" ) { continue; }
                if ( fields.at(6) != "PASS" || regex_match (fields.at(7), std::regex(".*INDEL.*") ))  { continue; }
                // homozygous ref call
                else if ( fields.at(7).substr(0,3) == "END" ){
                    aline.start = std::stoi(fields.at(1));
                    aline.end = std::stoi( fields.at(7).substr(4)  );
                    aline.ref = true;
                    
                    if( contigs.count(fields.at(0)) == 0  ){
                        vcf_region new_region;
                        new_region.contig = fields.at(0);
                        new_region.lines.push_back(aline);
                        contigs.insert( std::pair <std::string, vcf_region> (fields.at(0), new_region));
                        
                    }
                    else{
                        contigs.at(fields.at(0)).lines.push_back(aline);
                    }
                    
                    
                    
                    continue;
                }
                // 1bp homozygous ref blocks
                else if ( fields.at(7).substr(0,5) == "MinDP" ){
                    aline.start = std::stoi(fields.at(1));
                    aline.end = std::stoi( fields.at(1)  );
                    aline.ref = true;
               
               
                    if( contigs.count(fields.at(0)) == 0  ){
                        vcf_region new_region;
                        new_region.contig = fields.at(0);
                        new_region.lines.push_back(aline);
                        contigs.insert( std::pair <std::string, vcf_region> (fields.at(0), new_region));
               
                    }
                    else{
                        contigs.at(fields.at(0)).lines.push_back(aline);
                    }
               
               
               
                    continue;
                }
                // SNP call
                else{
                    
                    std::string genotype = (gfField == "PL") ? get_genotype_PL(fields) : get_genotype_GT(fields);
                    
                    aline.start = std::stoi(fields.at(1));
                    aline.end = std::stoi( fields.at(1)  );
                    try{
                     aline.alt = toIUPAC(genotype);
                     }
                     catch(...){
                         if(iupacStrict){
                             std::cerr << "ERROR (toIUPAC) in file " << infile << " on  vcf line: " << cline << std::endl;
                             exit(1);
                             
                         }else{
                             std::cerr << "WARNING (toIUPAC failed - masked with N) in file " << infile << " on  vcf line: " << cline << std::endl;
                             aline.alt = 'N';
                         }
                     }
                    aline.ref = false;
                    if( contigs.count(fields.at(0)) == 0  ){
                        vcf_region new_region;
                        new_region.contig = fields.at(0);
                        new_region.lines.push_back(aline);
                        contigs.insert( std::pair <std::string, vcf_region> (fields.at(0), new_region));
                        
                    }
                    else{
                        contigs.at(fields.at(0)).lines.push_back(aline);
                    }
                    continue;
                }
            }
            catch(...){
                //std::clog <<"Failed to parse VCF line: "  <<  cline ;
                failed_parse++;
                //throw "Failed to parse VCF line: " + cline ;
            }
            
        }
    }
    fh_infile.close();
}


std::string vcf::get_genotype_PL ( const std::vector <std::string > & fields){

    // alternative alleles
    std::vector <std::string> alt_alleles = msplit( fields.at(4), "," );

    // check where phred-scaled likelihood is stored
    unsigned int pl_field=0;
    bool found_pl = false;
    std::vector <std::string> keys = msplit( fields.at(8), ":" );
    for(pl_field=0; pl_field < keys.size(); pl_field++){
        if( strncmp(keys.at(pl_field).c_str(), "PL", 2) == 0 ){
            found_pl = true;
            break;
        }
    }
    if(!found_pl){ throw std::string("Cannot find PL field"); }
    
    std::vector <std::string> values = msplit( fields.at(9), ":" );
    std::vector <std::string> pl_values = msplit( values.at(pl_field), "," );
    
    int min_pl_field = -1 , num_zeros = 0;
    for( unsigned int index_pl = 0; index_pl < pl_values.size(); index_pl++ ){
        if ( std::stoi(pl_values.at(index_pl)) == 0 ){
            min_pl_field = index_pl;
            num_zeros++;
        }
    }
    if( num_zeros != 1 ){ throw "WARNING: more than 1 PL field is 0"; }
    
    else{
        // min_pl_field, alt_alleles
        std::string geno;
        // check input first
        if(   (alt_alleles.size() == 1 && pl_values.size() != 3 )
           || (alt_alleles.size() == 2 && pl_values.size() != 6 )
           || (alt_alleles.size() == 3 && pl_values.size() != 10 )
           || (alt_alleles.size() == 4 && pl_values.size() != 15 )
           
           ){
            throw std::string("Number of PL fields and alt alleles does not match.");
        }
        else{
            switch(min_pl_field){
                case 0: // RR
                    geno = fields.at(3) + fields.at(3);
                    //toreturn.push_back(geno);
                    return geno;
                    break;
                case 1: //RA1
                    geno = fields.at(3) + alt_alleles.at(0);
                    return geno;
                    break;
                case 2: //A1A1
                    geno = alt_alleles.at(0) + alt_alleles.at(0);
                    return geno;
                    break;
                case 3: //RA2
                    geno = fields.at(3) + alt_alleles.at(1);
                    return geno;
                    break;
                case 4: //A1A2
                    geno = alt_alleles.at(0) + alt_alleles.at(1);
                    return geno;
                    break;
                case 5: // A2A2
                    geno = alt_alleles.at(1) + alt_alleles.at(1);
                    return geno;
                    break;
                case 6: // RA3
                    geno = fields.at(3) + alt_alleles.at(2);
                    return geno;
                    break;
                case 7: // A1A3
                    geno = alt_alleles.at(0) + alt_alleles.at(2);
                    return geno;
                    break;
                case 8:  // A2A3
                    geno = alt_alleles.at(1) + alt_alleles.at(2);
                    return geno;
                    break;
                case 9:  // A3A3
                    geno = alt_alleles.at(2) + alt_alleles.at(2);
                    return geno;
                    break;
                case 10:
                    geno = fields.at(3) + alt_alleles.at(3);
                    return geno;
                    break;
                case 11:
                    geno = alt_alleles.at(0) + alt_alleles.at(3);
                    return geno;
                    break;
                case 12:
                    geno = alt_alleles.at(1) + alt_alleles.at(3);
                    return geno;
                    break;
                case 13:
                    geno = alt_alleles.at(2) + alt_alleles.at(3);
                    return geno;
                    break;
                case 14:
                    geno = alt_alleles.at(3) + alt_alleles.at(3);
                    return geno;
                    break;
                default:
                    throw std::string("Genotype unclear");
            }
        }
    }
}


std::string vcf::get_genotype_GT ( const std::vector <std::string > & fields){
    
    // alternative alleles
    std::vector <std::string> alt_alleles = msplit( fields.at(4), "," );
    std::vector <std::string> all_alleles;
    all_alleles.push_back(fields.at(3));
    for(auto i:alt_alleles){
        all_alleles.push_back(i);
    }
    
    // check where GT is stored
    unsigned int gt_field=0;
    bool found_gt = false;
    std::vector <std::string> keys = msplit( fields.at(8), ":" );
    for(gt_field=0; gt_field < keys.size(); gt_field++){
        if( strncmp(keys.at(gt_field).c_str(), "GT", 2) == 0 ){
            found_gt = true;
            break;
        }
    }
    
    if(!found_gt){ throw std::string("Cannot find GT field"); }
    
    std::vector <std::string> values = msplit( fields.at(9), ":" );
    std::vector <std::string> gt_values = msplit( values.at(gt_field), "/" );  // this will not work for phased vcfs
    if(gt_values.size() != 2){ throw std::string("GT field does not have 2 values separated by /"); }
    std::string geno = all_alleles.at(std::stoi(gt_values.at(0))) + all_alleles.at(std::stoi(gt_values.at(1)));
    return geno;
}



char vcf::toIUPAC (const std::string instr){
    std::string in = instr;
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);
    if( in.at(0) == in.at(1) ){
        return in.at(0);
    }
    else if ( (in.at(0) == 'a' && in.at(1) == 'c') || (in.at(1) == 'a' && in.at(0) == 'c') ){
        return 'm';
    }
    else if ( (in.at(0) == 'a' && in.at(1) == 'g') || (in.at(1) == 'a' && in.at(0) == 'g') ){
        return 'r';
    }
    else if ( (in.at(0) == 'a' && in.at(1) == 't') || (in.at(1) == 'a' && in.at(0) == 't') ){
        return 'w';
    }
    else if ( (in.at(0) == 'c' && in.at(1) == 'g') || (in.at(1) == 'c' && in.at(0) == 'g') ){
        return 's';
    }
    else if ( (in.at(0) == 't' && in.at(1) == 'g') || (in.at(1) == 't' && in.at(0) == 'g') ){
        return 'k';
    }
    else if ( (in.at(0) == 'c' && in.at(1) == 't') || (in.at(1) == 'c' && in.at(0) == 't') ){
        return 'y';
    }
    else{
        throw("");
    }
}


std::string vcf::make_fas ( std::string contig_name, std::string reference_seq ){
    if( contigs.count(contig_name) == 0){
        std::string res( reference_seq.size(), 'N' );
        return res;
    }
    else{
        std::string res( reference_seq.size(), 'N' );
        for ( unsigned int ivar = 0; ivar < contigs.at(contig_name).lines.size()  ; ivar++ ) {
            if( contigs.at(contig_name).lines.at(ivar).ref == true ){
                res.replace(contigs.at(contig_name).lines.at(ivar).start - 1
                            ,  contigs.at(contig_name).lines.at(ivar).end - contigs.at(contig_name).lines.at(ivar).start + 1
                            , reference_seq.substr( contigs.at(contig_name).lines.at(ivar).start - 1, contigs.at(contig_name).lines.at(ivar).end - contigs.at(contig_name).lines.at(ivar).start + 1 ) );
            }
            else{
                res.at( contigs.at(contig_name).lines.at(ivar).start - 1  ) = contigs.at(contig_name).lines.at(ivar).alt;
            }
        }
        return res;
    }
    
}

int vcf::get_total_variants_read(){
    int tot = 0;
    std::map< std::string, vcf_region >::iterator it = contigs.begin();
    for (it=contigs.begin(); it!=contigs.end(); ++it)
        tot += it->second.lines.size();
    return tot;
}


void vcf::speak(){
    std::cout << "Infile: " << infile << std::endl << "N contigs: " << contigs.size() << std::endl << "N vars per contig:" << std::endl;
    std::map< std::string, vcf_region >::iterator it = contigs.begin();
    for (it=contigs.begin(); it!=contigs.end(); ++it)
        std::cout << it->first << " : " << it->second.lines.size() << '\n';
}
