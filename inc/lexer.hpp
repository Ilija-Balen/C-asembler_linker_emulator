#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
using namespace std;

class Lexer{
  std::list <string> tokens;
   
  public:
  std::list <string> tokenize(string line);
  Lexer(){};
};


#endif