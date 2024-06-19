//
//  main.cpp
//  args_hashes
//
//  Created by Bruno on 27/07/2023.
// my simple implementation of command line argument reader / v2 (with any number and type of optional arguments)

#include <cstdio>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

struct smargs {
    std::unordered_map< std::string, int > args_int;
    std::unordered_map< std::string, float > args_float;
    std::unordered_map< std::string, bool > args_bool;
    std::unordered_map< std::string, std::string > args_str;
};

class margs{
private:
    smargs args;
    std::vector <std::string> args_defined;
public:
    void getargs (int argc, const char * argv[], std::vector <std::string> args);
    int getInt(std::string key){return this->args.args_int.at(key); };
    float getFloat(std::string key){return this->args.args_float.at(key); };
    bool getBool(std::string key){return this->args.args_bool.at(key); };
    bool isArgDefined(std::string key){ return (std::find(this->args_defined.begin(), this->args_defined.end(), key) != this->args_defined.end() ) ? true : false; };
    
    
    std::string getString(std::string key){return this->args.args_str.at(key); };
};

void margs::getargs(int argc, const char * argv[], std::vector <std::string> args){
    
    if( argc % 2 == 0){
        throw std::string("Command malformed (expected even number of keys/values)!");
    }
    // read in pairs of args key/values
    std::unordered_map<std::string, std::string> rawargs;
    for(unsigned int i = 1; i < argc; i += 2){
        //std::cout << "received: " << argv[i] << std::endl;
        std::string s(argv[i]);
        
        if( s.at(0) != '-'){
            throw std::string("Argument pair malformed: " + s + " " + std::string(argv[i+1]));
        }
        s.erase(s.begin());
        if(rawargs.count(s) > 0){
            throw std::string("Argument defined more than once: " + s);
        }
        rawargs[s] = std::string(argv[i+1]);
    }
    
    // now go over requested args
    // map below is just to check whether args appear more than once
    std::unordered_map<std::string, int> checkArgsRequested;
    for (auto &i: args){
        std::stringstream ss(i);
        std::string arg;
        std::vector <std::string> vcarg;
        while (getline(ss, arg, ',')){
            vcarg.push_back(arg);
        }
        //these are sanity checks for the call to args, not for the input to main program
        if(vcarg.size() != 3 || (vcarg.at(2) != "t" && vcarg.at(2) != "f")){
            throw std::string("Argument definition wrong (this is a coding issue): " + vcarg.at(0));
        }
        if(checkArgsRequested.count(vcarg.at(0)) > 0 ){
            throw std::string("Argument defined multiple times (this is a coding issue): " + vcarg.at(0));
        }
        checkArgsRequested[vcarg.at(0)]=1;
        
        // Now process current arg (name,type,optional);
        
        // string argument
        if(vcarg.at(1) == "s"){
            if(rawargs.count(vcarg.at(0)) == 0 && vcarg.at(2) == "f"){
                throw std::string("Required argument -" + vcarg.at(0) + " missing");
            }else if (rawargs.count(vcarg.at(0)) == 1){
                this->args.args_str[vcarg.at(0)] = rawargs.at(vcarg.at(0));
                this->args_defined.push_back(vcarg.at(0));
            }
            rawargs.erase(vcarg.at(0));
            
        // int argument
        }else if (vcarg.at(1) == "i"){
            if(rawargs.count(vcarg.at(0)) == 0 && vcarg.at(2) == "f"){
                throw std::string("Required argument -" + vcarg.at(0) + " missing");
            }else if (rawargs.count(vcarg.at(0)) == 1){
                try{
                    std::size_t pos{0};
                    this->args.args_int[vcarg.at(0)] = std::stoi(rawargs.at(vcarg.at(0)), &pos);
                    this->args_defined.push_back(vcarg.at(0));
                    if(pos != rawargs.at(vcarg.at(0)).size()){
                        throw "";
                    }
                }
                catch(...){
                    throw std::string("Failed to convert to int argument pair -" + vcarg.at(0) + " " + rawargs.at(vcarg.at(0)) );
                }
            }
            rawargs.erase(vcarg.at(0));
        // float arg
        }else if (vcarg.at(1) == "f"){
            if(rawargs.count(vcarg.at(0)) == 0 && vcarg.at(2) == "f"){
                throw std::string("Required argument -" + vcarg.at(0) + " missing");
            }else if (rawargs.count(vcarg.at(0)) == 1){
                try{
                    this->args.args_float[vcarg.at(0)] = std::stof(rawargs.at(vcarg.at(0)));
                    this->args_defined.push_back(vcarg.at(0));
                }
                catch(...){
                    throw std::string("Failed to convert to float argument pair -" + vcarg.at(0) + " " + rawargs.at(vcarg.at(0)) );
                }
            }
            rawargs.erase(vcarg.at(0));
        // boolean arg
        }else if (vcarg.at(1) == "b"){
            if(rawargs.count(vcarg.at(0)) == 0 && vcarg.at(2) == "f"){
                throw std::string("Required argument -" + vcarg.at(0) + " missing");
            }else if (rawargs.count(vcarg.at(0)) == 1){
                this->args.args_bool[vcarg.at(0)] = (rawargs.at(vcarg.at(0)) == "0") ? false : true;
                this->args_defined.push_back(vcarg.at(0));
                
            }
            rawargs.erase(vcarg.at(0));
        }else {
            throw std::string("Dont know what type of arg this is: " + vcarg.at(1));
        }
    }
    // now cheeck that no undefined arguments requested by user remain
    if(rawargs.size() != 0){
        std::cerr << "Unknown arguments supplied by user:" << std::endl;
        for ( std::pair<const std::string, std::string>& n : rawargs){
            std::cerr << "-" << n.first << " " << n.second << endl;
        }
        throw std::string ("Unknown arguments supplied by user");
    }
    
}



/* example below

int main(int argc, const char * argv[]) {

    margs programOptions;
    try{
        programOptions.getargs(argc, argv, std::vector <std::string> {
            "a1,s,f",
            "a2,s,t",
            "a3,i,f",
            "a4,i,t",
            "a5,f,f",
            "a6,f,t",
            "a7,b,f",
            "a8,b,t"});
    }catch(std::string e){std::cerr << "Failed reading args: " << e << std::endl;exit(1);}
    cout << "done" << endl;
    
    
    bool infile1 = programOptions.isArgDefined("a8") ? programOptions.getBool("a8") : false;
    
    std::cout << " infile 1 is " << infile1 << std::endl;
    
    return 0;
}
*/
