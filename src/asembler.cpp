#include "../inc/asembler.hpp"

void Asembler::asemble(string ulaz, string izlaz){
  
  std::ifstream inputFile(ulaz); 
  std::ofstream outputFile(izlaz);
    //cout<<"ZDRAVO"<<endl;
  if (!outputFile.is_open()) {
      std::cerr << "Error opening the file for writing." << std::endl;
  }
  if (!inputFile.is_open()) {
      std::cerr << "Error opening the file" << std::endl;
  }

    std::string line;
    while (std::getline(inputFile, line)) {
         if (line.size() == 0 || line==" "||line=="\n" || line=="\t" || line.empty() || line.front()=='#') {
          continue;
      }

      if(prviProlaz(line)) break;
    }
    
    // Move the read pointer to the beginning of the file
    inputFile.seekg(0, std::ios::beg);

    while (std::getline(inputFile, line)) {
         if (line.size() == 0 || line==" "||line=="\n" || line=="\t" || line.empty() || line.front()=='#') {
          continue;
      }
      if(drugiProlaz(line)) break;
    }

    
    symtable->ispis();
    std::cout<< std::endl<< std::endl;
    for(auto rel: allRelTables)rel->ispis();
    std::cout<< std::endl<< std::endl;
    sectiontable->ispis(); 

    symtable->upisi(&outputFile);
    for(auto rel:allRelTables){
      rel->upis(&outputFile);
    }
    sectiontable->upisi(&outputFile);

    inputFile.close();
    outputFile.close();
}

bool Asembler::prviProlaz(string line){
  std::list<string> lista = lexer->tokenize(line);
  
  Parsed parsed = parser->parse(lista);
  
  switch (parsed.id)
  {
    case empty:{}
      break;
  case direktiva:{

    switch (parsed.direktiv)
    {
    case global:{//nista
    }
      
      break;

    case exterN:{
      for(auto argument : parsed.listaArg){
        if(symtable->ifExists(argument) == -1){
          symtable->addSymbol(locCounter, GLOBAL, 0, argument, NOTYPE);//value, bind, Ndx, name, Type (Und, SCTN)
        }else{
          cout<<"GRESKA EXTERN SIMBOL POSTOJI U TABELI SIMBOLA";
        }
      
      }
    }

      break;
    
    case section:{
      int oldlocCounter = locCounter;
      if(sekcija!= nullptr)popuniLitTable(oldlocCounter);
      locCounter = 0;
      string argument = parsed.listaArg.front();
      if(symtable->ifExists(argument) == -1){
        if(sekcija != nullptr){
          //nije prva sekcija koju smo otvorili, treba da namestimo size atribut sekcije u tabeli simbola
          symtable->setSizeSekcije(sekcija->name, oldlocCounter - sekcija->base);   
        } 
        newSection(argument); //sekcija pokazuje na novu sekciju 
        symtable->addSymbol(locCounter, LOCAL, sekcija->NDX, parsed.listaArg.front(), SCTN);
      }
    }

      break;
    
    case word:{
      for(auto arg : parsed.listaArg) {//za svaki argument po 4 bajta
        locCounter+=4;
        ifHexDecaddLitProvera(arg);
      } 
    }

      break;
    
    case skip:{
      int preskoci = std::stoi(parsed.listaArg.front());
      locCounter+= preskoci;
    }

      break;
    
    case enD:{
      //postavi size atribut poslednje sekcije
      symtable->setSizeSekcije(sekcija->name, locCounter - sekcija->base);          
      popuniLitTable(locCounter);
      currSection = 0;
      locCounter = 0;
      sekcija = nullptr;
      return true;
    }

      break;
    
    default:
      break;
    }
  }
    break;
    //INSTRUKCIJA-------------------------------------------------------------------
    //------------------------------------------------------------------------------
  case instrukcija:{
    locCounter+=4;
    switch (parsed.inst)
    {
    case ld:{
      string arg = parsed.listaArg.front();
      ifHexDecaddLitProvera(arg);
    }
      break;
    case st:{
      string arg = parsed.listaArg.back();
      ifHexDecaddLitProvera(arg);
    }
      break;
    case call:
    case jmp:{
      string arg = parsed.listaArg.front();
      ifHexDecaddLitProvera(arg);
    }
      break;
    case beq:
    case bne:
    case bgt:{//na trecem mestu moguc literal ali i to je kraj (back) 
      string arg = parsed.listaArg.back();
      ifHexDecaddLitProvera(arg);
    }
      break;
    
    default:
      break;
    }
  }
    break;
  case labela:{
    string arg = parsed.labela;
    if(symtable->ifExists(arg) == -1){
          symtable->addSymbol(locCounter, LOCAL, sekcija->NDX, arg, NOTYPE);//value, bind, Ndx, name, Type (Und, SCTN)
    }
  }
    break;
  default: {cout<< "Nije prepoznata direktiva prvi prolaz assemble()";}
    break;
  }

  return false;
}



//DRUGI PROLAZ ASEMBLER------------------------------------------------------------
//---------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
bool Asembler::drugiProlaz(string line){
   std::list<string> lista = lexer->tokenize(line);
  Parsed parsed = parser->parse(lista);
  if(line == "" || line == " " || line == "\t" || line == "\n") return false;
  switch (parsed.id)
  {
  case direktiva:{

    switch (parsed.direktiv)
    {
    case global:{
      for(string argument : parsed.listaArg){
        if(symtable->ifExists(argument) != -1){
          symtable->setGlobal(argument);
        }else {
          symtable->addSymbol(locCounter, GLOBAL, currSection, argument, NOTYPE);
        }
      }
    }
      
      break;

    case exterN:{

    }

      break;
    
    case section:{
      locCounter = 0;
      sekcija = sectiontable->getSection(parsed.listaArg.front());
      currSection++;
      //otvori relTable za ovu sekciju
      currRelTable = new RelTable();
      currRelTable->setName(sekcija->name);
      allRelTables.push_back(currRelTable);
    }

      break;
    
    case word:{
      for(auto arg : parsed.listaArg){
        //da li je literal
        if(isDecimalLiteral(arg)){
          int decimalValue = std::stoi(arg);
          sectiontable->addDecimal(sekcija, decimalValue);
        }
        else if(isHexadecimalLiteral(arg)){
          //remove first 2 letters
          //string pomarg = arg.substr(2);
          
          //hex string to int
          int decimalValue = std::stoi(arg, nullptr, 16);

          sectiontable->addDecimal(sekcija, decimalValue);
        }else {
          //simbol
          Sym * sym = symtable->getSymbol(arg);
          if(sym->bind == GLOBAL){
            sectiontable->addDecimal(sekcija, 0);
  
            Rela* zapis = new Rela();            
            zapis->r_offset = locCounter;
            zapis->r_type = ABS;
            zapis->r_addend = 0;
            zapis->s_index = sym->RBr;

            currRelTable->dodajZapis(zapis);
          }else{//lokalan je
            //dohvati vrednost iz tabele simbola i stavi je u sekciju
            int decimalValue = symtable->getSymbol(arg)->value;
            sectiontable->addDecimal(sekcija, decimalValue);

            //zatim novi relokacioni zapis ciji je addend vrednost simbola
            Rela* zapis = new Rela();
            zapis->r_offset = locCounter;
            zapis->r_type = ABS;
            zapis->r_addend = decimalValue;
            //lokalan mora da se stavi index sekcije
            zapis->s_index = symtable->getSymbol(sekcija->name)->RBr;
            
            currRelTable->dodajZapis(zapis);
          }
        }
        locCounter+=4;
      }
    }

      break;
    
    case skip:{
      int decimalValue = stoi(parsed.listaArg.front());
      locCounter += decimalValue;

      sectiontable->skip(sekcija, decimalValue);
    }

      break;
    
    case enD:{
      sectiontable->dodajTabeleLiteralaUSadrzaj();
      sectiontable->setSize(symtable);
      currSection = 0;
      locCounter = 0;
      sekcija = nullptr;
      return true;
    }

      break;
    
    default:
      break;
    }
  }
    break;
  //INSTRUKCIJA-------------------------------------------------------------------
    //------------------------------------------------------------------------------
  case instrukcija:{
    switch (parsed.inst)
    {
    //instrukcije skoka i poziva potprograma
        //call, jmp, beq, bne, bgt
      case call:{//call operand
        string arg = parsed.listaArg.front();
        int lokacija = getPostavljenLocLitOrSymb(arg);

        lokacija-=locCounter;
        int slokacija = unsignedToSigned12Bit(lokacija);
        string pomeraj = intToHexString(slokacija);
        if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

        popuniSadrzajInstr("21", 15, 0, 0, pomeraj);
      }
        break;
      case jmp:{//jmp operand
        string arg = parsed.listaArg.front();
        int lokacija = getPostavljenLocLitOrSymb(arg);

        lokacija-=locCounter;
        int slokacija = unsignedToSigned12Bit(lokacija);
        string pomeraj = intToHexString(slokacija);
        if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

        popuniSadrzajInstr("38", 15, 0, 0, pomeraj);
      }
        break;
      case beq:
      case bne:
      case bgt:{//beq %r1, %r2, handle_software
        string arg3 = parsed.listaArg.back();
        string arg2;
        int brojac = 0;
        for(string a : parsed.listaArg){
          if(brojac==1)arg2 = a;
          brojac++;
        }
        string arg = parsed.listaArg.front();

        int lokacija = getPostavljenLocLitOrSymb(arg3);
        lokacija-=locCounter;
        int slokacija = unsignedToSigned12Bit(lokacija);
        string pomeraj = intToHexString(slokacija);
        if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
            
          }
          else pomeraj= "000";

        int regB = extractRegNumb(arg); 
        int regC = extractRegNumb(arg2);

        string opCode;
        if(parsed.inst == beq) opCode ="39";
        if(parsed.inst == bne) opCode ="3a";
        if(parsed.inst == bgt) opCode ="3b";
        popuniSadrzajInstr(opCode, 15, regB, regC, pomeraj);
      }
        break;

      //instrukcije za rad sa podacima
      //ld, st
      case ld:{
        switch (parsed.tipadresiranja)
        {
        case vrednost_mem:{//ld value2, %r1
          //"92"
          string arg = parsed.listaArg.front();
          int lokacija = getPostavljenLocLitOrSymb(arg);

          int regA = extractRegNumb(parsed.listaArg.back());
          lokacija-=locCounter;
          int slokacija = unsignedToSigned12Bit(lokacija);
          string pomeraj = intToHexString(slokacija);
          if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

          if(isHexadecimalLiteral(arg) || isDecimalLiteral(arg)) popuniSadrzajInstr("92", regA, 0, 15, pomeraj);
          else popuniSadrzajInstr("98", regA, 0, 15, pomeraj);
        }
          break;

        case reg:{//ld %r1, %sp
          //"91"
          int regA = extractRegNumb(parsed.listaArg.back());
          int regB = extractRegNumb(parsed.listaArg.front());

          popuniSadrzajInstr("91", regA, regB, 0, "000");
        }
          break;

        case vrednost:{//ld $0xFFFFFEFE, %sp
          string arg = parsed.listaArg.front();
          int lokacija = getPostavljenLocLitOrSymb(arg);
          
          int regA = extractRegNumb(parsed.listaArg.back());
          lokacija-=locCounter;
          int slokacija = unsignedToSigned12Bit(lokacija);
          string pomeraj = intToHexString(slokacija);
          if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

          popuniSadrzajInstr("92", regA, 0, 15, pomeraj);
        }
          break;

        case mem_reg:{//ld [%r1], %r2
          int regA = extractRegNumb(parsed.listaArg.back());
          int regB = extractRegNumb(parsed.listaArg.front());

          popuniSadrzajInstr("92", regA, regB, 0, "000");
        }
          break;

        case mem_reg_pom:{//ld [%sp + 0x08], %r1
          string arg = parsed.listaArg.front();
          string arg2;

          int brojac = 0;
          for(string a: parsed.listaArg){
            if(brojac == 1) arg2 = a;
            brojac++;
          }
        
          int regA = extractRegNumb(parsed.listaArg.back());
          int regB = extractRegNumb(arg);
          
          int value;
          std::stringstream ss;
          ss << std::hex << arg2;
          ss >> value;
          if(value > 4095) {cout<<"vrednost literala nije moguće zapisati na širini od 12 bita";}
          string pomeraj = intToHexString(value);
          if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

          popuniSadrzajInstr("92", regA, regB, 0, pomeraj);

        }
          break;

        default:
          break;
        }
      }
        break;

      case st:{
        switch (parsed.tipadresiranja)
        {
        case vrednost_mem:{ //st %r1, value2
          string arg2 = parsed.listaArg.back();
          int lokacija;

          lokacija = getPostavljenLocLitOrSymb(arg2);
          
          int regC = extractRegNumb(parsed.listaArg.front());
          int regA = 15;
          lokacija-=locCounter;
          int slokacija = unsignedToSigned12Bit(lokacija);
          string pomeraj = intToHexString(slokacija);
          if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

          if(isHexadecimalLiteral(arg2) || isDecimalLiteral(arg2)) popuniSadrzajInstr("80", regA, 0, regC, pomeraj);
          else popuniSadrzajInstr("82", regA, 0, regC, pomeraj);//DODAO
        }
          break;
        case reg:{
        //nije moguce
        } 
          break;
        case vrednost:{
          //nije moguce
        }
          break;
        case mem_reg:{//st %r1, [%r2]

          int regC = extractRegNumb(parsed.listaArg.front());
          int regA = extractRegNumb(parsed.listaArg.back());
          popuniSadrzajInstr("80", regA, 0, regC, "000");
        }
          break;
        case mem_reg_pom:{//nije moguce da se desi pomeraj sa simbolom
          // st %r1, [%sp + 0x08] ovo moguce
          string arg3 = parsed.listaArg.back();
          string arg2;
          int brojac = 0;
          for(string a : parsed.listaArg){
            if(brojac==1) arg2 = a;
            brojac++;
          }

          int lokacija;
          if(isDecimalLiteral(arg3)){
            long long decimalValue = std::stoll(arg3);
            lokacija = sekcija->getValueLiteral(decimalValue, arg3); 
          }
          if(isHexadecimalLiteral(arg3)){
              //remove first 2 letters
            //arg3 = arg3.substr(2);
            // Convert hexadecimal string to integer
            long long decimalValue = std::stoll(arg3, nullptr, 16);
            lokacija = sekcija->getValueLiteral(decimalValue, arg3);
          }

          int regC = extractRegNumb(parsed.listaArg.front());
          int regA = extractRegNumb(arg2);
          lokacija-=locCounter;
          int slokacija = unsignedToSigned12Bit(lokacija);
          string pomeraj = intToHexString(slokacija);
          if(pomeraj!="0"){
            if(pomeraj.size()==1)pomeraj = "00"+pomeraj;
            if(pomeraj.size()==2)pomeraj = "0"+pomeraj;
            if(pomeraj.size() >3)pomeraj = pomeraj.substr(pomeraj.size()-3);
          }
          else pomeraj= "000";

          popuniSadrzajInstr("80", regA, 0, regC, pomeraj);
        }
          break;
        default:
          break;
        }
      }
        break;
      
      //instrukcije sa jednim operandom (registrom)
      //push, pop, not
      case push:{
        //"81" u A ide r14 (14 u hex), u D -4 (ffc), u C operand
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = 14;
        popuniSadrzajInstr("81", regA, 0, regC, "ffc");
      }
        break;
      case pop:{
        //"93" u A ide operand, u B ide r14, u D ide 4 (4)
        int regB = 14;
        int regA = extractRegNumb(parsed.listaArg.front());
        popuniSadrzajInstr("93", regA, regB, 0, "004");
      }
        break;
      case noT:{
        //"60" u AB jedini
        int regA = extractRegNumb(parsed.listaArg.front());
        popuniSadrzajInstr("60", regA, regA, 0, "000");
      }
        break;

      //instrukcije sa dva operanda (registra)
      //xchg, add, sub, mul, div, and, or, xor, shl, shr
      case xchg:{//atomicna zamena vrednosti
        //"400" + BBBB + CCCC + "000" B treba da bude drugi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regB = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("40", 0, regB, regC, "000");
      }
        break;
      case add:{
        //AAAA = drugi i u B = drugi i u C = prvi
        //"50"
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("50", regA, regA, regC, "000"); 
      }
        break;
      case sub:{
        //A drugi, B drugi, C PRVI
        //"51"
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("51", regA, regA, regC, "000"); 
      }
        break;
      case mul:{
        //A drugi, B drugi, C PRVI
        //"52"
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("52", regA, regA, regC, "000"); 
      }
        break;
      case diV:{
        //A drugi, B drugi, C PRVI
        //"53"
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("53", regA, regA, regC, "000"); 
      }
        break;
      //LOGICKE OPERACIJE, fali NOT
      case anD:{
        //"61"  u AB drugi, u C prvi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("61", regA, regA, regC, "000"); 
      }
        break;
      case oR:{
        //"62" u AB drugi, u C prvi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("62", regA, regA, regC, "000");
      }
        break;
      case xoR:{
        //"63" u AB drugi, u C prvi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("63", regA, regA, regC, "000");
      }
        break;
      case shl:{
        //"70" u AB drugi, C prvi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("70", regA, regA, regC, "000");
      }
		break;
      case shr:{
        //"71" u AB drugi, C prvi
        int regC = extractRegNumb(parsed.listaArg.front());
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("71", regA,regA, regC, "000");
      }
        break;
       
       //instrukcije sa dva registra, jedan je programski dostupan
      case csrrd:{
        //"90" u A ide drugi operand, u B ide prvi ali je CSR
        string CSR = parsed.listaArg.front();
        int regB;
        if(CSR == "status"){
          regB = 0;
        }else if (CSR == "handler"){
          regB = 1;
        }else if (CSR == "cause"){
          regB = 2;
        }else {cout<<"u CSR los operand nije status/handler/cause \n";}
        
        int regA = extractRegNumb(parsed.listaArg.back());
        popuniSadrzajInstr("90", regA, regB, 0, "000");
      }
        break;
      case csrwr:{
        //"94" u A ide drugi operand koji je CSR, u B ide prvi
        string CSR = parsed.listaArg.back();
        int regA;
        if(CSR == "status"){
          regA = 0;
        }else if (CSR == "handler"){
          regA = 1;
        }else if (CSR == "cause"){
          regA = 2;
        }else {cout<<"u CSR los operand nije status/handler/cause \n";}
        
        int regB = extractRegNumb(parsed.listaArg.front());
        popuniSadrzajInstr("94", regA, regB, 0, "000");
      }
        break;

      //instrukcije bez operanada
      //halt, int, iret, ret
      case halt:{
        sectiontable->addDecimal(sekcija, 0);
      }
        break;
      case Int:{
        popuniSadrzajInstr("01", 0,0,0,"000");
      }
        break;
      case iret:{//pop PC, pop STATUS
        int regB = 14;
        int regA = 15;
        popuniSadrzajInstr("99", regA, regB, 0, "004");
      }
        break;
      case ret:{//pop PC
        //"93" u A ide PC, u B ide r14, u D ide 4 (4)
        int regB = 14;
        int regA = 15;
        popuniSadrzajInstr("93", regA, regB, 0, "004");
      }
        break;
    default:
      break;
    }
    locCounter+=4;
  }
    break;
  case labela:{}
    break;
  default: {cout<< "Nije prepoznata direktiva drugi prolaz assemble()";}
    break;
  }

  return false;
}

void Asembler::newSection(string imeSekcije){
  Section* nova_sekcija = new Section(); 
  nova_sekcija->name = imeSekcije;
  nova_sekcija->base = locCounter;
  currSection++;
  nova_sekcija->NDX = currSection;
  sectiontable->addSection(nova_sekcija);
  sekcija = nova_sekcija;
}

bool Asembler::isDecimalLiteral(const std::string& str) {
    return std::regex_match(str, std::regex(R"(\d+)"));
}

bool Asembler::isHexadecimalLiteral(const std::string& str) {
    return std::regex_match(str, std::regex(R"(0x[0-9a-fA-F]+)"));
}

int Asembler::extractRegNumb(string s){
  if(s == "sp") return 14;
  s = s.substr(1);
  return stoi(s);
}

string Asembler::intToHexString(int br){
  std::stringstream ss;
  ss << std::hex << br;
  std::string hexString = ss.str();
  
  return hexString;
}

int Asembler::addLiteralTableSymbol(long long key, string name){
    int val;
    if(sekcija->tabela_literala.size()!=0){
      val = sekcija->tabela_literala.back()->adress;
      sekcija->addLiteral(key, val+4, name);
      return val+4;
    
    }else {//tabela sekcija je prazna, a naisli smo na simbol
      //alocira se novi prostor na kraju sekcije
      val = symtable->getSymbol(sekcija->name)->size;
      sekcija->addLiteral(key, val, name);
      return val;
    }
     
    
}

void Asembler::addLiteralTableProvera(long long key, int val, string ime){
  if(sekcija->getValueLiteral(key, ime) == NO_LITERAL){
    sekcija->addLiteral(key, val, ime);
  }
}

void Asembler::ifHexDecaddLitProvera(string arg){
  if(isHexadecimalLiteral(arg)){
    //remove first 2 letters
    //arg = arg.substr(2);
    // Convert hexadecimal string to integer
    long long decimalValue = std::stoll(arg, nullptr, 16);
    addLiteralTableProvera(decimalValue, -1, arg);
  }
  if(isDecimalLiteral(arg)){
    // Convert string to integer
    long long decimalValue = std::stoll(arg);
    addLiteralTableProvera(decimalValue, -1, arg);
  }
}

void Asembler::popuniLitTable(int oldLocCounter){
  for(auto literal: sekcija->tabela_literala){
    literal->adress = oldLocCounter;
    oldLocCounter+=4;
  }
}

void Asembler::popuniSadrzajInstr(string opCode, int regA, int regB, int regC, string pomeraj){
  
  string sregC = intToHexString(regC);
  string sregB = intToHexString(regB);
  string sregA = intToHexString(regA);

  string hexString = opCode + sregA + sregB + sregC + pomeraj;
  sectiontable->addString(sekcija, hexString);
}

int Asembler::unsignedToSigned12Bit(int unsignedValue) {
    if (unsignedValue > 0xFFF) {
      std::cout<<"Input value must be between 0 and 4095 (0xFFF) inclusive.";
      
    }

    int signedValue;
    if (unsignedValue <= 0x7FF) {
        signedValue = unsignedValue; // No sign change needed for positive values
    } else {
        signedValue = static_cast<int>(unsignedValue) - 0x1000; // Convert to negative signed value
    }

    return signedValue;
}

int Asembler::getPostavljenLocLitOrSymb(string arg2){
  int lokacija;
  if(isHexadecimalLiteral(arg2)){
    
    // Convert hexadecimal string to integer
    long long decimalValue = std::stoll(arg2, nullptr, 16);
    lokacija = sekcija->getValueLiteral(decimalValue,arg2);
    
  }else if(isDecimalLiteral(arg2)){

    long long decimalValue = std::stoll(arg2);
    lokacija = sekcija->getValueLiteral(decimalValue, arg2);
    
  }else{
    //simbol
    int decimalValue = symtable->getSymbol(arg2)->value;
    if(sekcija->getValueLiteral(decimalValue, arg2) == NO_LITERAL){
      //ne postoji vrednost u tabeli literala
      lokacija = addLiteralTableSymbol(decimalValue, arg2);
    }
    lokacija = sekcija->getValueLiteral(decimalValue, arg2);
    //URADI dodaj u RelTable zapis
    addRelZapisInstr(lokacija, arg2);    
  }
  return lokacija;
}

void Asembler::addRelZapisInstr(int lokacija, string simbol){
  Rela* zapis = new Rela();            
  zapis->r_offset = lokacija;
  zapis->r_type = ABS;
  zapis->r_addend = 0;

  Sym * sym = symtable->getSymbol(simbol);
  if(sym->bind == GLOBAL){
    zapis->s_index = sym->RBr;
  }else {
    //lokalan mora da se stavi index sekcije u kojoj se nalazi taj simbol
    zapis->s_index = symtable->getSymbol(sekcija->name)->RBr;
    zapis->r_addend = sym->value;
  }

  currRelTable->dodajZapis(zapis);
}


int main(int argc, char* argv[]){

  if(argc < 4){
    cout<< "NISTE UNELI ODGOVARAJUCI FORMAT PRI POKRETANJU ASEMBLERA";
    return -1;
  }
  string s = argv[1];
  if(s != "-o"){
    cout<<"ULAZ ASEMBLERA MORA POCINJATI SA -o" << argv [1];
    return -1;
  } 

  Asembler* asembler = new Asembler();
  asembler->asemble(argv[3], argv[2]);
  return 0;
}