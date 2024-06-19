

#include "fasta.h"

void fasta::readFastaFile(std::string infile, bool aligned, bool verbose){
    
    std::ifstream readFile(infile);
    if(verbose){
        std::cout << "Reading sequences from fasta file " << infile << " ... " << std::flush;
    }
    if(!readFile.is_open()){
        throw std::string("ERROR: cant open infile?");
    }
    std::string cline;
    std::string csequence;
    while (getline (readFile, cline)) {
        if( cline.at(0) == '>' ){
            if( this->doesNameExist( cline.substr(1) ) ){
                throw std::string ("ERROR: repeated sequence names in infile");
            }
            this->sequenceNames.push_back(cline.substr(1));
            if( csequence.length() > 0 ){
                this->sequenceData.push_back(csequence);
                csequence.erase();
            }
        }
        else{
            csequence.append(cline);
        }
    }
    this->sequenceData.push_back(csequence);

    if(aligned){
        for(int i = 1; i < this->sequenceData.size(); i++){
            if(this->sequenceData.at(i).length() != this->sequenceData.at(0).length() ){
                throw std::string("ERROR: sequence data from infile is not aligned");
            }
        }
    }
    if(this->sequenceData.size() != this->sequenceNames.size()){
        throw std::string("ERROR: number of names and sequences does not match in file?");
    }

    if(verbose){
        std::cout << "done, read " << this->sequenceData.size() << " sequences" << std::endl;
    }
}


void fasta::writeFastaFile(std::string outfile, bool verbose){
    std::ofstream writeFile(outfile);
     if(verbose){
        std::cout << "Writing fasta format to file " << outfile << " : " << this->sequenceData.size() << " sequences ... "  << std::flush;
    }
    if(!writeFile.is_open()){
        throw std::string ("ERROR: cant open file for output?");
    }

    for( int i = 0; i < this->getNumberOfSequences(); i++ ){
        writeFile << ">" << this->sequenceNames.at(i) << std::endl << this->sequenceData.at(i) << std::endl;
    }
    if(verbose){
        std::cout << " done" << std::endl;
    }
    writeFile.close();

}


void fasta::addSequence (std::string name, std::string seq, bool checkAlign, bool verbose){
    if(verbose){
        std::cout << "Adding sequence named " << name << " to data ... " << std::flush;  
    }
    if( checkAlign && seq.length() != this->sequenceData.at(0).size() ){
        throw std::string ("ERROR: sequence does not have correct length?");
    }
    else if( this->doesNameExist( name ) ) {
        throw std::string ("ERROR: sequence name to add already exists");
    }
    else{
        this->sequenceNames.push_back(name);
        this->sequenceData.push_back(seq);
    }
        if(verbose){
            std::cout << "done" << std::endl;  
    }
}


bool fasta::doesNameExist(std::string name){
    for(auto &i : this->sequenceNames ){
        if( i == name){
            std::cerr << "Name appears more than once: " << name << std::endl; 
            return true;
        }
    }
    return false;
}
