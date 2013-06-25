/*
 *  util.h
 *  
 *
 *  Created by Xin Li and Fei Song on 11-08-11.
 *  Copyright 2011 University of Guelph. All rights reserved.
 *
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;

//bool onString( char ch, string word );

vector<string> tokenize( string words, string delimiters );

#endif
