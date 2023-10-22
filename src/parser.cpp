#include "../inc/parser.hpp"

Parsed Parser::parse(std::list<string> tokens)
{
  Parsed parsed;
  //remove all blank signs
  tokens.remove("");
  tokens.remove(" ");
  if(tokens.size() == 0){
    parsed.id = empty;
    return parsed;
  }
  
  try
  {
    
    //labela?
     if(tokens.back().back() == ':'){
      int positionToStop = tokens.size()-1;
      int counter = 0;
      for (std::list<string>::iterator it = tokens.begin(); it != tokens.end() && counter < positionToStop; ++it) {
        if(*it != "") {cout<<"Pre labele ne sme biti nista"; throw "Pre labele ne sme biti nista";}
        counter++;
      }

      //uspesno dosli do ovde znaci da pre labele nema nicega i da je labela ustvari na prvom kao i poslednjem mestu jer su izbaceni blanko znaci
      parsed.labela = tokens.front().substr(0, tokens.front().size()-1);
      parsed.id = labela;
      return parsed;
    }else
   if(tokens.front().front()== '.'){
      //direktiva
      parsed.id= direktiva;

      string directive = tokens.front();
      if (directive.size() > 1) {
          directive = directive.substr(1);
        }


      if (directive == "global")
      {
        parsed.direktiv= Direktiv::global;
      }else if (directive == "extern"){
        parsed.direktiv= Direktiv::exterN;
      }else if (directive == "section"){
        parsed.direktiv= Direktiv::section;
      }else if (directive == "word"){
        parsed.direktiv= Direktiv::word;
      }else if (directive == "skip"){
        parsed.direktiv= Direktiv::skip;
      }else if (directive == "end"){
        parsed.direktiv= Direktiv::enD;
      }else {cout<< "direktiva ne postoji" ; throw "direktiva ne postoji";}

      //sledi prikupljanje argumenata za direktivu
      //ZAVRTEO SAM DVE PETLJE
      switch (parsed.direktiv)
      {
      case global:{
        tokens.remove(",");
        int c = 0;
        for(string it: tokens){
            if(c== 0) {c++;continue;}
            parsed.listaArg.push_back(it);
          }
        }
        break;
      case exterN:{
        tokens.remove(",");

        int c = 0;
        for(string it: tokens){
            if(c== 0) {c++;continue;}
            parsed.listaArg.push_back(it);
          }
        }
      
        break;
        case section:{
          if(tokens.size()>2) { cout<<"greska, section moze da ima samo ime";throw "greska, section moze da ima samo ime";}
          parsed.listaArg.push_back(tokens.back());
        }
        break;
        case word:{
          tokens.remove(",");
        int c = 0;
        for(string it: tokens){
            if(c== 0) {c++;continue;}
            parsed.listaArg.push_back(it);
          }
        }
        break;
        case skip:{
          if(tokens.size()>2) {cout<<"greska, skip moze da ima samo jedan arg" ;throw "greska, skip moze da ima samo jedan arg";}
          parsed.listaArg.push_back(tokens.back());
        }
        break;
        case enD:{
          if(tokens.size()>1) { cout<<"greska, end nema arg"; throw "greska, end nema arg";}
        }
        break;
      default:{cout<< "ne postoji naredba"; throw "ne postoji naredba";}
        break;
      }
      
    }else {
      //naredba je u pitanju
      parsed.id = instrukcija;
      
      string naredba = tokens.front();
      
      if(naredba == "halt"){
        parsed.inst = halt;
      }else if(naredba == "int"){
        parsed.inst = Int;
      }else if(naredba == "iret"){
        parsed.inst = iret;
      }else if(naredba == "call"){
        parsed.inst = call;
      }else if(naredba == "ret"){
        parsed.inst = ret;
      }else if(naredba == "jmp"){
        parsed.inst = jmp;
      }else if(naredba == "beq"){
        parsed.inst = beq;
      }else if(naredba == "bne"){
        parsed.inst = bne; 
      }else if(naredba == "bgt"){
        parsed.inst = bgt;
      }else if(naredba == "push"){
        parsed.inst = push;
      }else if(naredba == "pop"){
        parsed.inst = pop;
      }else if(naredba == "xchg"){
        parsed.inst = xchg;
      }else if(naredba == "add"){
        parsed.inst = add;
      }else if(naredba == "sub"){
        parsed.inst = sub;
      }else if(naredba == "mul"){
        parsed.inst = mul;
      }else if(naredba == "div"){
        parsed.inst = diV;
      }else if(naredba == "not"){
        parsed.inst = noT;
      }else if(naredba == "and"){
        parsed.inst = anD;
      }else if(naredba == "or"){
        parsed.inst = oR;
      }else if(naredba == "xor"){
        parsed.inst = xoR;
      }else if(naredba == "shl"){
        parsed.inst = shl;
      }else if(naredba == "shr"){
        parsed.inst = shr;
      }else if(naredba == "ld"){
        parsed.inst = ld;
      }else if(naredba == "st"){
        parsed.inst = st;
      }else if(naredba == "csrrd"){
        parsed.inst = csrrd;
      }else if(naredba == "csrwr"){
        parsed.inst = csrwr;
      }

      //sledi prikupljanje argumenata za instrukciju (naredbu)
      switch (parsed.inst)
      {
        //instrukcije skoka i poziva potprograma
        //call, jmp, beq, bne, bgt
      case call:
      case jmp:
      case beq:
      case bne:
      case bgt:{
        int n = tokens.size();
        switch (n)
        {

        case 2:{ //slucaj kad je call 0x700023 (ili call handle_timer)
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 1);
          parsed.listaArg.push_back(*it);
        }
          break;
        case 8:{//slucaj kad je reg, reg, operand beq %r1, %r2, handle_timer
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 2); // na 2

          parsed.listaArg.push_back(*it);
          std::advance(it, 3); // na 5
          parsed.listaArg.push_back(*it);
          std::advance(it, 2); // na 7
          parsed.listaArg.push_back(*it);
        }
          break;
        
        default: { cout<<"nije dobar broj operanada SKOK";throw "nije dobar broj operanada SKOK";}
          break;
        }
      }
        break;
      //instrukcije za rad sa podacima
      //ld, st
      case ld:
      case st:{
        int n = tokens.size();
        switch (n)
        {
        case 5:{//st %r1, value2 ili ld value2, %r1
          //ispravi zbog ld i st
          if(parsed.inst == ld){

            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 1);//1
            parsed.listaArg.push_back(*it);
            std::advance(it, 3);//4
            parsed.listaArg.push_back(*it);

          }else if(parsed.inst == st){

            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 2);//2
            parsed.listaArg.push_back(*it);
            std::advance(it, 2);//4
            parsed.listaArg.push_back(*it);

          }

          parsed.tipadresiranja = vrednost_mem;
        }
          break;
    
        case 6:{//ld $0xFFFFFEFE, %sp ili st %sp, $0xFFFFFFFEF ili ld %r1, %sp
          std::list<string>::iterator it1 = tokens.begin();
          std::advance(it1, 1);//1
          string prov1 = *it1;
          std::advance(it1, 3);//4
          string prov2 = *it1;
          if(prov1 != "$" && prov2 != "$") parsed.tipadresiranja = reg;
          else parsed.tipadresiranja = vrednost;
          
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 2);//2
          parsed.listaArg.push_back(*it);
          std::advance(it, 3);//5
          parsed.listaArg.push_back(*it);
        }
          break;
        case 8:{//st %r1, [%r2] ili ld [%r1], %r2
            std::list<string>::iterator it1 = tokens.begin();
          std::advance(it1, 1);//1
          string prov1 = *it1;
          std::advance(it1, 3);//4
          string prov2 = *it1;
          if(prov1 != "[" && prov2 != "[") {cout<< "reg_mem Losi podaci LD,ST";throw "reg_mem Losi podaci LD,ST";}
            

          if(prov1 == "["){
            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 3);//3
            parsed.listaArg.push_back(*it);
            std::advance(it, 4);//7
            parsed.listaArg.push_back(*it);
          }else if(prov2 == "["){
            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 2);//2
            parsed.listaArg.push_back(*it);
            std::advance(it, 4);//6
            parsed.listaArg.push_back(*it);
          }
          
          parsed.tipadresiranja = mem_reg;
        }
          break;
        case 10:{//ld [%sp + 0x08], %r1 ili st %r1, [%sp + 0x08]

          if(parsed.inst == ld){

            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 3);//3
            parsed.listaArg.push_back(*it);
            std::advance(it, 2);//5
            parsed.listaArg.push_back(*it);
            std::advance(it, 4);//9
            parsed.listaArg.push_back(*it);

          }else if(parsed.inst == st){

            std::list<string>::iterator it = tokens.begin();
            std::advance(it, 2);//2
            parsed.listaArg.push_back(*it);
            std::advance(it, 4);//6
            parsed.listaArg.push_back(*it);
            std::advance(it, 2);//8
            parsed.listaArg.push_back(*it);
          }

          parsed.tipadresiranja = mem_reg_pom;
        }
          break;
        default:{ cout<<"LD,ST,losi podaci";throw "LD, ST losi podaci";}
          break;
        }
      }
        break;
      //instrukcije sa jednim operandom (registrom)
      //push, pop, not
      case push:
      case pop:
      case noT:{
        int n = tokens.size();
        if(n==3){
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 2);
          parsed.listaArg.push_back(*it);
        }else {cout<<"nije dobar broj operanada Jedan REG" ;throw "nije dobar broj operanada Jedan REG";}
      }
        break;
      //instrukcije sa dva operanda (registra)
      //xchg, add, sub, mul, div, and, or, xor, shl, shr
      case xchg:
      case add:
      case sub:
      case mul:
      case diV:
      case anD:
      case oR:
      case xoR:
      case shl:
      case shr:{ //mul %r2, %r1
        int n = tokens.size();
        if(n==6){
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 2);//2
          parsed.listaArg.push_back(*it);
          std::advance(it, 3);//5
          parsed.listaArg.push_back(*it);
        }else {cout<<"nije dobar broj operanada DVA REG" ;throw "nije dobar broj operanada DVA REG";}
      }
        break;
      
      //instrukcije sa dva registra, jedan je programski dostupan
      case csrrd:
      case csrwr:{//csrwr %r1, %handler
        int n = tokens.size();
        if(n==6){
          std::list<string>::iterator it = tokens.begin();
          std::advance(it, 2);//2
          parsed.listaArg.push_back(*it);
          std::advance(it, 3);//5
          parsed.listaArg.push_back(*it);
        }else {cout<<"nije dobar broj operanada 2 REG, jedan programski" ;throw "nije dobar broj operanada 2 REG, jedan programski";}
      }
        break;
      
      //instrukcije bez operanada
      //halt, int, iret, ret
      case halt:
      case Int:
      case iret:
      case ret:{
        int n = tokens.size();
        if(n!=1){ cout<<"nije dobar broj operanada 0 Operanada";throw "nije dobar broj operanada 0 Operanada";}
      }
        break;
      
      default:
        break;
      }
    }
    
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  


  return parsed;
}
