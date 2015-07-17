

#include "args.h"


sargs args::getargs ( int argc, const char * argv[], const std::vector <std::string > & args_string, const std::vector <std::string > & args_bool, const std::vector <std::string > & args_int, const std::string & args_string_opt, const std::string & args_int_opt ){
    
    sargs return_args;
    if( argc % 2 == 0){
        throw std::string("command malformed (expected even number of keys/values)!");
    }
    for (unsigned int is = 0; is < args_string.size(); is++) {
        std::string tofind = "-";
        tofind.append(args_string.at(is));
        bool found = false;
        for ( int iarg = 0; iarg < argc; iarg++) {
            if( argv[iarg] == tofind ){
                return_args.args_string.push_back(argv[iarg+1]);
                found = true;
                break;
            }
        }
        if (!found){
            throw "Required argument '" + args_string.at(is) + "' missing!";
        }
    }
    for (unsigned int ii = 0; ii < args_int.size(); ii++) {
        std::string tofind = "-";
        tofind.append(args_int.at(ii));
        bool found = false;
        for ( int iarg = 0; iarg < argc; iarg++) {
            if( argv[iarg] == tofind ){
                try{
                    return_args.args_int.push_back( std::stoi (argv[iarg+1]) );
                }
                catch(...){
                    throw "Argument '" + args_int.at(ii) + "' should be numeric (received: " + argv[iarg+1] + ")!";
                }
                found = true;
                break;
            }
        }
        if (!found){
            throw "Required argument '" + args_int.at(ii) + "' missing!";
        }
        
    }
    for (unsigned int is = 0; is < args_bool.size(); is++) {
        std::string tofind = "-";
        tofind.append(args_bool.at(is));
        bool found = false;
        for ( int iarg = 0; iarg < argc; iarg++) {
            if( argv[iarg] == tofind ){
                
                if( argv[iarg+1] == std::string("0") ){
                    return_args.args_booleans.push_back(false);
                    found = true;
                    break;
                }
                
                else if (argv[iarg+1] == std::string("1")){
                    return_args.args_booleans.push_back(true);
                    found = true;
                    break;
                }
                else{
                    throw  "Boolean argument '" + args_bool.at(is) + "' must be 0 or 1!";
                }
                
            }
        }
        if (!found){
            throw "Required argument '" + args_bool.at(is) + "' missing!";
        }
        
    }
    // OPTIONAL ARGS. Currently only 1 of each type (string and int) allowed
    
    //for (unsigned int is = 0; is < args_string_opt.size(); is++) {
        std::string tofind_str = "-";
        tofind_str.append(args_string_opt);
        for ( int iarg = 0; iarg < argc; iarg++) {
            if( argv[iarg] == tofind_str ){
                return_args.args_string_optional.push_back(argv[iarg+1]);
                break;
            }
        }
    //}
   // for (unsigned int ii = 0; ii < args_int_opt.size(); ii++) {
        std::string tofind_int = "-";
        tofind_int.append(args_int_opt);
        for ( int iarg = 0; iarg < argc; iarg++) {
            if( argv[iarg] == tofind_int ){
                try{
                    return_args.args_int_optional.push_back( std::stoi (argv[iarg+1]) );
                }
                catch(...){
                    throw "Argument '" + args_int_opt + "' should be numeric (received: " + argv[iarg+1] + ")!";
                }
                break;
            }
        }

   // }
    return return_args;
}


