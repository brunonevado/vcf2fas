//
//  common.cpp
//  vcf2fas
//
//  Created by brunonevado on 03/10/2014.
//

#include "common.h"

std::vector <std::string>  msplit( const std::string& s , std::string delim ){
    std::vector <std::string> res;
    unsigned long start = 0U;
    unsigned long end = s.find(delim);
    while (end != std::string::npos)
    {
        res.push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
    }
    res.push_back(s.substr(start, end - start));
    return res;
}

