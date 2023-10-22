#include "../inc/lexer.hpp"

std::list<string> Lexer::tokenize(string line)
{
  std::list <string> pomlist;
  string token = "";
  
  for(auto character : line){
    if(line == " ")break;
    if(character == '#') break;
    std::regex slovo("[a-zA-Z0-9._]+");
    std::regex blanko("[ \\t\\n]+");
    std::regex other("[^a-zA-Z0-9._ \\t\\n]+");
    
    std::string characterStr(1,character);
    if(std::regex_match(characterStr, slovo)){
    
      token+=character;
    
    }else if(std::regex_match(characterStr, blanko)){
    
      pomlist.push_back(token);
      token ="";

    }else if(std::regex_match(characterStr, other)){
     
      if(token!= ""){
        if(character==':')token+=character;
        else pomlist.push_back(token);
      }
      if(character!=':'){
        token=character;
        pomlist.push_back(token);
        token="";
      }
    }
  }

  if(token!= "") pomlist.push_back(token);
  return pomlist;
}
