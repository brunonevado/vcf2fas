
#include <vector>
#include <iostream>
#include <fstream>
#include <string>


class fasta {
    private:
        std::vector <std::string> sequenceData; 
        std::vector <std::string> sequenceNames; 
        bool doesNameExist(std::string name);
        std::string versionCaller;
    public:
        fasta( std::string version ) {this->versionCaller = version;};
        void readFastaFile(std::string infile, bool aligned = false, bool verbose = false);
        void writeFastaFile(std::string outfile, bool verbose = false);

        int getNumberOfSequences ()  { return this->sequenceData.size(); }
        const std::string & getSequence (int i) const { return this->sequenceData.at(i); }
        std::string getSequenceName (int i)  { return this->sequenceNames.at(i); }

        void addSequence (std::string name, std::string seq, bool checkAlign = true, bool verbose = false);

};