/*
 B. Nevado, 01/10/2014
 My implementation of a minimal class to read arguments from command line
 
 
 Currently will look for required args of type int, string, float and bool
 
Example usage (Requires C++11):
 
 #include "args.h"
 //get args
 ////////////////////
 sargs myargs;
 try{
  myargs = args::getargs(argc, argv, std::vector<std::string> {"infile", "outfile"}, std::vector<std::string> {}, std::vector<std::string>  {}, std::string {}, std::string {}); }
 catch (std::string e){
 std::cout << " Args failed: " << e << std::endl;
 exit(1);
 }
 
 std::cout << "Infile: " << myargs.args_string.at(0) << ", outfile: " 
           << myargs.args_string.at(1) << std::endl;

////////////////////
 
TODO: recode optional args: now is only OK with 1 optional arg (and int/string)
 
 */



#ifndef __args__
#define __args__

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream> 


struct sargs {
    std::vector <std::string > args_string;
    std::vector <int > args_int;
    std::vector <float > args_float;
    std::vector <bool> args_booleans;
    std::vector <std::string > args_string_optional;
    std::vector <int > args_int_optional;
    //std::vector <float > args_float_optional;
    //std::vector <bool> args_booleans_optional;
    
};


class args {
public:
    sargs static getargs ( int argc, const char * argv[],
                          const std::vector <std::string > & args_string,
                          const std::vector <std::string > & args_bool,
                          const std::vector <std::string > & args_int,
                          const std::string & args_string_opt,
                          const std::string & args_int_opt
           
                          );
    
    
};


#endif /* defined(__args__) */
