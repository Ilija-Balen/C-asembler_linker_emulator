#include "../inc/linker.hpp"

void Linker::mapiranje(std::vector<Placeholder>* placeholder){
  long long adresa = pocetnaAdresa;

  for(int i = 0; i < datoteke.size(); i++){
    std::vector<string> sec_header_names;
     allSecTables[i]->sectiontable->getHeaderNames(&sec_header_names);
    std::vector<int> sec_header_ndx;
    allSecTables[i]->sectiontable->getHeaderNDX(&sec_header_ndx);
     
    if(i==0){
        
      for(int j = 0 ; j < sec_header_names.size(); j++){
        //prodji kroz sve sekcije datoteke i
        for(int k = 0; k < placeholder->size(); k++){
          if((*placeholder)[k].ime_sekcije == sec_header_names[j]){
            adresa = stoll((*placeholder)[k].hex_adresa, nullptr, 16);
            break;
          }
        }
      
        Podatak* podatak = new Podatak(); podatak->input_number = i; podatak->name = sec_header_names[j];
        podatak->size = allSymTables[i]->symtable->getSymbol(podatak->name)->size;
        if(mapirano[adresa] == nullptr){
          mapirano[adresa] = new NizPodataka();
          mapirano[adresa]->niz.push_back(podatak);
        }else {
          cout<< "POSTOJI PREKLAPANJE IZMEDJU SEKCIJA";
        }
        //prodji kroz podatke vezane za sekciju
        adresa+= mapiraj_podatke_vezane_za_sekciju(adresa, sec_header_ndx[j], i,podatak->name);       
      }
    }else{
      
      for(int j = 0 ; j < sec_header_names.size(); j++){
        //prodji kroz sve sekcije datoteke i
        //proveri da li je sekcija mapirana, ukoliko jeste nova se lepi na staru
        //i siftuju se sve na gore
        if(isSectionMapped(sec_header_names[j])){
          adresa = mapiraj_Mapped(sec_header_names[j], i);
          mapiraj_podatke_vezane_za_sekciju(adresa, sec_header_ndx[j], i,sec_header_names[j]);
        
        }else if(isInPlaceholder(sec_header_names[j], placeholder)){
          adresa = mapiraj_InPlaceholder(sec_header_names[j], i, placeholder);
          mapiraj_podatke_vezane_za_sekciju(adresa, sec_header_ndx[j], i ,sec_header_names[j]);
        
        }else {
          auto it = --mapirano.end();
          adresa = pronadji_poslednji_mapirani(i-1);
          Podatak* podatak = new Podatak(); podatak->input_number = i; podatak->name = sec_header_names[j];
          podatak->size = allSymTables[i]->symtable->getSymbol(podatak->name)->size;
          if(mapirano[adresa] == nullptr){
            mapirano[adresa] = new NizPodataka();
            mapirano[adresa]->niz.push_back(podatak);
          }else {
            cout<< "POSTOJI PREKLAPANJE IZMEDJU SEKCIJA";
            
          }
          //prodji kroz podatke vezane za sekciju
          mapiraj_podatke_vezane_za_sekciju(adresa, sec_header_ndx[j], i,podatak->name);       
      
        }
        //ako nije, proverava se da li se sekcija nalazi u placeholderu
        //ako se ne nalazi u placeholderu krece od poslednje mapiranje adrese

      }
    }
    //sad je sve iz input1 mapirano osim und symbola
    dodajUndSibole(i);
  }
  proveri_preklapanje();
}

void Linker::odredjivanje(){
  
  std::vector <string> all_sec_header_names;
  getAllSHDRNamesNoRepetition(&all_sec_header_names);
  //imamo imena sekcija, bez ponavljanja
  
  //sad treba da prolazimo kroz imena sekcija i da odredimo za svaku sekciju size
  //i koja joj je pocetna adresa
  //pocetna adresa joj je prvo pojavljivanje u mapirano
  //a size joj je zbir sizeova svih pojavljivanja  
  for(int i = 0; i < all_sec_header_names.size(); i++){
    finalSymTable->addSymbol(-1, LOCAL, i+1, all_sec_header_names[i],SCTN);
    Sym * sym = finalSymTable->getSymbol(all_sec_header_names[i]);
    sym->size = 0;
    for (auto it = mapirano.begin(); it != mapirano.end(); ++it) {
      //idemo redom  
      if(it->second->niz[0]->name == sym->name && sym->value == -1){
        sym->value = static_cast<int>(it->first);
        sym->size += it->second->niz[0]->size; 
      }else if(it->second->niz[0]->name == sym->name){
        sym->size += it->second->niz[0]->size;
      }
    }    
  }
  //sad imamo postavljene sve sectione u tabelu simbola
  //sad treba da prodjemo kroz sve mapirane podatke i njih da stavimo u finalSymTable
  for (auto it = mapirano.begin(); it != mapirano.end(); ++it) {
      //idemo redom  
      if(it->second->niz.size()>1){
        for(int i = 0; i < it->second->niz.size(); i++){
          if(finalSymTable->getSymbol(it->second->niz[i]->name) == nullptr){
            //nije jos u tabeli simbola, dodaj ga
            Sym* sym1 = allSymTables[it->second->niz[i]->input_number]->symtable->getSymbol(it->second->niz[i]->name);
            string naziv_sekcije_kojoj_pripada = allSymTables[it->second->niz[i]->input_number]->symtable->nazivSekcijeKojojPripada(it->second->niz[i]->name); 
            int ndx_trenutne_sekcije = finalSymTable->getSymbol(naziv_sekcije_kojoj_pripada)->NDX;
            int vr_pom = static_cast<int>(it->first);
            finalSymTable->addSymbol(vr_pom, sym1->bind, ndx_trenutne_sekcije, it->second->niz[i]->name,NOTYPE);
          } 
        }
      }else if(finalSymTable->getSymbol(it->second->niz[0]->name) == nullptr){
        //nije jos u tabeli simbola, dodaj ga
        Sym* sym1 = allSymTables[it->second->niz[0]->input_number]->symtable->getSymbol(it->second->niz[0]->name);
        string naziv_sekcije_kojoj_pripada = allSymTables[it->second->niz[0]->input_number]->symtable->nazivSekcijeKojojPripada(it->second->niz[0]->name); 
        int ndx_trenutne_sekcije = finalSymTable->getSymbol(naziv_sekcije_kojoj_pripada)->NDX;
        int vr_pom = static_cast<int>(it->first);
        finalSymTable->addSymbol(vr_pom, sym1->bind, ndx_trenutne_sekcije, it->second->niz[0]->name,NOTYPE);
      }
    }
    //prosli kroz sve podatke i stavili ih u finalsymTable
    //treba proci kroz nedefinisane simbole i videti jel su u tabeli simbola
    //ako nisu prijaviti gresku
    for (int i = 0; i< undefinedSymbolNames.size(); i++){
      if(finalSymTable->getSymbol(undefinedSymbolNames[i]->name)== nullptr){
        cout<<"POSTOJI SIMBOL KOJI NIJE DEFINISAN";
      }
    } 
}

void Linker::razresavanje(){
  //prvo napuni memoriju podacima iz hex_tabela
  napuni_memoriju();
  napuni_sekcije_sa_ponavljanjem();
  //treba proci kroz sve relokacione zapise 
  //uzeti njihov s_index, uzeti iz tabele simbola tog relokacionog zapisa
  //ulaz ciji je RBR == s_indexu i pronaci ga u mapiranom
  //zatim S = it->first
  //treba da nadjemo i P (place) = 
  //imenu relokacionog zapisa (inputa) + ofset tog relokacionog zapisa
  // i na to mesto u memoriji memorija[P] stavljamo S+addend

  for(int i = 0; i < datoteke.size(); i++){
    for(auto j: allRelTables[i]->table){
      for(auto k : j->ulazi){
        long long S = pronadji_u_mapiranom(allSymTables[i]->symtable->getSymbolNameByRBR(k->s_index), i);
        long long P = pronadji_u_mapiranom(j->sectionName, i);
        P+= k->r_offset;
        S+= k->r_addend;
        
        std::stringstream stream;
        stream << std::hex << S;  // Convert integer to hexadecimal format
        std::string hex_string = stream.str();
        hex_string = allSecTables[0]->sectiontable->dopuniString(hex_string);

        int pomeraj = 7;
        string pom ="";
        for(int a = 0; a < 4; a++){
          int pomeraj1 = pomeraj-1;
          pom+=hex_string[pomeraj1];
          pom+=hex_string[pomeraj];
          pomeraj-=2;
          //u little endianu 
        }
        memorija[P] = pom;
      }
    }

  }

}

void Linker::ucitaj(std::vector<string> vektorDatoteka){
  for(int i = 0 ; i < vektorDatoteka.size(); i++){
    string currDat = vektorDatoteka[i];

    allSymT * currSymTable = new allSymT();
    currSymTable->fileName = currDat;
    allSymTables.push_back(currSymTable);
 
    allRelT * currRelTable = new allRelT();
    currRelTable->fileName = currDat;
    allRelTables.push_back(currRelTable);
    RelTable * trenutniRelTable;
    

    allSecT * currSecTable = new allSecT();
    currSecTable->fileName = currDat;
    allSecTables.push_back(currSecTable);
    Section* trenutniSec;

    std::ifstream inputFile(currDat); 
    if (!inputFile.is_open()) {
      std::cerr << "Error opening the file. And" << std::endl;
    }

    //obrada
    std::string line;
    int cntREL = 0;
    int cntSEC = 0;
    bool indikatorSYM = false;
    bool indikatorREL = false;
    bool indikatorSEC = false;
    while (std::getline(inputFile, line)) {
      //    if (line.size() == 0 || line==" "||line=="\n" || line=="\t" || line.empty() || line.front()=='#') {
      //     continue;
      // }
      if(line == "SYMTABLE"){indikatorSYM = true;  indikatorREL = false;  indikatorSEC = false;cntREL=0; cntSEC=0; continue;}
      if(line == "RELTABLE"){indikatorSYM = false; indikatorREL = true;   indikatorSEC = false; cntREL=0; cntSEC=0; continue;}
      if(line == "SECTABLE"){indikatorSYM = false; indikatorREL = false;  indikatorSEC = true;cntREL=0; cntSEC=0; continue;}
      if(line == "tabela_literala") {cntSEC=2; continue;}
      if(line == "hex_sadrzaj"){cntSEC=1; continue;}

      if(indikatorSYM){
        std::vector<std::string> values;

        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            values.push_back(token);
        }
        currSymTable->symtable->addLine(stoi(values[0]), stoi(values[1]), stoi(values[2]), stoi(values[3]), stoi(values[4]), stoi(values[5]), values[6]);
      }

      if(indikatorREL){
        if(cntREL == 0){//Namestanje nove relTabele u niz Reltabela (my_section; other_section...)
          cntREL = 1;trenutniRelTable = new RelTable();trenutniRelTable->setName(line);
          currRelTable->table.push_back(trenutniRelTable);continue;
        }

        std::vector<std::string> values;

        std::istringstream iss(line);
        std::string token;

        while (iss >> token) {
            values.push_back(token);
        }
        if(line == "" || line == "\n"){cntREL=0;continue;}
        trenutniRelTable->addLine(stoi(values[0]), stoi(values[1]), stoi(values[2]), stoi(values[3]));
      }

      if(indikatorSEC){
          std::vector<std::string> values;

          std::istringstream iss(line);
          std::string token;
          while (iss >> token) {
              values.push_back(token);
          }
        if(cntSEC == 0){
          trenutniSec = new Section();
          currSecTable->sectiontable->addLine(trenutniSec, stoi(values[0]), values[1],stoi(values[2]), stoi(values[3]));         
          cntSEC = 1;

        }else if(cntSEC == 1){//TODO ukoliko je sekcija 1 da se pododaje hex_sadrzaj
          if(line == "" || line == "\n"){cntSEC=0;continue;}
          if(values.size()==1) currSecTable->sectiontable->addLineHex(trenutniSec, values[0], "");
          else currSecTable->sectiontable->addLineHex(trenutniSec, values[0], values[1]);
          

        }else if(cntSEC == 2){
        //ukoliko je sekcija 2 da se pododaje tabela_literala
          if(line == "" || line == "\n"){cntSEC=0;continue;}
          currSecTable->sectiontable->addLineLiteralTable(trenutniSec, stoll(values[0]), stoll(values[1]), values[2]);    
        }
      }
    }

    inputFile.close();
  }
}

int Linker::mapiraj_podatke_vezane_za_sekciju(long long adresa, int ndx, int rbr_input, string sec_name){
  std::vector<Sym> svi_simboli_sekcije;
  allSymTables[rbr_input]->symtable->getSviSymbolNDX(&svi_simboli_sekcije,ndx);
  int size;
  for(int i = 0; i < svi_simboli_sekcije.size(); i++){
    if(svi_simboli_sekcije[i].name != sec_name){
      
      Podatak* podatak = new Podatak(); podatak->name = svi_simboli_sekcije[i].name;
      podatak->input_number = rbr_input;
      if(mapirano[adresa + svi_simboli_sekcije[i].value] != nullptr)
        mapirano[adresa + svi_simboli_sekcije[i].value]->niz.push_back(podatak);
      else 
        {
          mapirano[adresa + svi_simboli_sekcije[i].value] = new NizPodataka();
          mapirano[adresa + svi_simboli_sekcije[i].value]->niz.push_back(podatak);
        }
    }else size = svi_simboli_sekcije[i].size; 
  }
  return size;
}

void Linker::dodajUndSibole(int rbr_input){
  std::vector<Sym> svi_simboli_sekcije;
  allSymTables[rbr_input]->symtable->getSviSymboli(&svi_simboli_sekcije);
  
  for(int i = 0; i < svi_simboli_sekcije.size(); i++){
    if(svi_simboli_sekcije[i].NDX == 0 && svi_simboli_sekcije[i].name != "0"){
      Podatak* p = new Podatak(); p->name =svi_simboli_sekcije[i].name;
      p->input_number = rbr_input;
      undefinedSymbolNames.push_back(p);
    }
  }
}

long long Linker::pronadji_poslednji_mapirani(int rbr_input){//iterira unazad
  std::vector<string> sec_header_names;
  allSecTables[rbr_input]->sectiontable->getHeaderNames(&sec_header_names);
  
  for (auto it = mapirano.rbegin(); it != mapirano.rend(); ++it) {
    for(int i = 0; i < sec_header_names.size(); i++){
      if(it->second->niz[0]->name == sec_header_names[i]){
      
      return it->first + allSymTables[rbr_input]->symtable->getSymbol(it->second->niz[0]->name)->size;
  
    } 
    }        
  }

  return 0;
}

bool Linker::isSectionMapped(string sec_name){
  
  for (auto it = mapirano.begin(); it != mapirano.end(); ++it) {
    if(it->second->niz[0]->name == sec_name) return true;
  }
  return false;
}

bool Linker::isInPlaceholder(string sec_name, std::vector<Placeholder>* placeholder){
  for(int k = 0; k < placeholder->size(); k++){
    if((*placeholder)[k].ime_sekcije == sec_name){
      return true;
    }
  }
  return false;
}

long long Linker::mapiraj_Mapped(string sec_name, int rbr_input){
  //ukoliko jeste nova se lepi na staru
  //i siftuju se sve na gore za size
  Sym* sym = allSymTables[rbr_input]->symtable->getSymbol(sec_name);
  
  long long key;
  int size_proslog;
  
  for (auto it = mapirano.rbegin(); it != mapirano.rend(); ++it) {
    if(it->second->niz[0]->name == sec_name) {
      key = it->first; size_proslog = it->second->niz[0]->size; break;
    }
  }
  //namestili smo key na vrednost istoimenesekcije kao i njen size
  key+= size_proslog;
  //sad imamo vrednost odakle treba da smestimo novu sekciju i njene podatke
  //i da siftujemo sve sto je od ove tacke za size ove sekcije
  int size_novog = sym->size;

  for (auto it = mapirano.rbegin(); it != mapirano.rend(); ++it) {
    //idemo od krajnjeg do trenutnog  
    if(it->first < key) break;
    auto next_iterator = std::next(it);  
    mapirano[it->first + size_novog] = mapirano[next_iterator->first];
  }
  //sad je sve shiftovano na gore
  Podatak * podatak = new Podatak(); podatak->name = sec_name; podatak->size = size_novog;
  podatak->input_number = rbr_input;
  mapirano[key] = new NizPodataka();
  mapirano[key]->niz.push_back(podatak);
  return key;
}

long long Linker::mapiraj_InPlaceholder(string sec_name, int rbr_input, std::vector<Placeholder>* placeholder){

  string hex_adresa;
  for(int k = 0; k < placeholder->size(); k++){
    if((*placeholder)[k].ime_sekcije == sec_name){
      hex_adresa = (*placeholder)[k].hex_adresa;
      break;
    }
  }
  long long adresa = std::stoll(hex_adresa, nullptr, 16);
  Podatak* podatak = new Podatak(); podatak->input_number = rbr_input; podatak->name = sec_name;
  podatak->size = allSymTables[rbr_input]->symtable->getSymbol(podatak->name)->size;
  if(mapirano[adresa] != nullptr){ 
    cout<< "GRESKA POSTOJI PREKLAPANJE " << adresa;}
  mapirano[adresa] = new NizPodataka();
  mapirano[adresa]->niz.push_back(podatak);
  return adresa;        
}

void Linker::proveri_preklapanje(){
  for (auto it = mapirano.begin(); it != mapirano.end(); ++it) {
    //idemo redom  
    auto next_iterator = std::next(it);
    for(; next_iterator != mapirano.end(); ++next_iterator){
      if(next_iterator->second->niz[0]->size != 0) break;
    }
    if(next_iterator == mapirano.end()) break;  
    if((it->first + it->second->niz[0]->size) > next_iterator->first)
      cout<< " GRESKA POSTOJI PREKLAPANJE -----------------------\n"<< it->first;
  }
}

void Linker::getAllSHDRNamesNoRepetition(std::vector<string>* all_sec_header_names){
  for(int i = 0; i< datoteke.size(); i++){
    std::vector<string> sec_header_names;
    allSecTables[i]->sectiontable->getHeaderNames(&sec_header_names);
    for(int j = 0; j< sec_header_names.size(); j++){
      if (std::find((*all_sec_header_names).begin(), (*all_sec_header_names).end(), sec_header_names[j]) != (*all_sec_header_names).end()) {
        //element fount
      }
      else {
          //element not found
          (*all_sec_header_names).push_back(sec_header_names[j]);
      }
    }
  }
}

void Linker::napuni_sekcije_sa_ponavljanjem(){
  std::vector<string> sekcije_bez_ponavljanja;
  
  for(int i = 0; i< datoteke.size(); i++){
    std::vector<string> sec_header_names;
    allSecTables[i]->sectiontable->getHeaderNames(&sec_header_names);
    for(int j = 0; j< sec_header_names.size(); j++){
      if (std::find((sekcije_bez_ponavljanja).begin(), (sekcije_bez_ponavljanja).end(), sec_header_names[j]) != (sekcije_bez_ponavljanja).end()) {
        //element found
        istoimeneSekcije.push_back(sec_header_names[j]);
      }
      else {
          //element not found
          (sekcije_bez_ponavljanja).push_back(sec_header_names[j]);
      }
    }
  }
}

void Linker::napuni_memoriju(){
  
  long long pomeraj = 0;

  for(int i = 0; i < allSecTables.size(); i++){
    for(int j = 0; j < allSecTables[i]->sectiontable->tabela.size(); j++){
      for (auto it = mapirano.begin(); it != mapirano.end(); ++it) {
        if(it->second->niz[0]->name == allSecTables[i]->sectiontable->tabela[j]->name && it->second->niz[0]->input_number == i){
          for(auto k = allSecTables[i]->sectiontable->tabela[j]->hex_sadrzaj.begin(); k != allSecTables[i]->sectiontable->tabela[j]->hex_sadrzaj.end(); ){
            string inputString = "";
            for(int z= 0 ; z< 4; z++){
              inputString+=*k;
              ++k;
            }
            memorija[it->first + pomeraj] = inputString;
            pomeraj+=4;
          }
          pomeraj = 0;
        }
      }
    }
  }
}

long long Linker::pronadji_u_mapiranom(string name, int rbr_input){
  //treba da prodjem kroz mapirano i da vidim da je jednak name i rbr_inputa

  if (std::find((istoimeneSekcije).begin(), (istoimeneSekcije).end(), name) != (istoimeneSekcije).end()) {
        //element found
      for(auto a: mapirano){
        if(a.second->niz[0]->name == name && a.second->niz[0]->input_number == rbr_input){
          return a.first;
        }
      }
      cout<< "GRESKA U PRONADJI_U_MAPIRANOM " << name <<" " << rbr_input;
  }
  else {
      //element not found
      for(auto a: mapirano){
        for(auto b: a.second->niz){
          if(b->name == name){
            return a.first;
          }
        }
      }
  }
  cout<< "GRESKA U PRONADJI_U_MAPIRANOM DOSLO DO KRAJA " << name <<" " << rbr_input;
  exit(1);
}

void Linker::upis(string ime_datoteke){
  std::ofstream outputFile(ime_datoteke);
    
  if (!outputFile.is_open()) {
      std::cerr << "Error opening the file for writing." << std::endl;
  }
  
  int i = 0;
  for(auto a: memorija){
    std::stringstream stream;
    stream << std::hex << a.first;  // Convert integer to hexadecimal format
    std::string hex_string = stream.str();
    hex_string = allSecTables[0]->sectiontable->dopuniString(hex_string);
    
    outputFile << hex_string<<":"<<" "<< a.second<<"\n";
  }

  
  outputFile.close();
}

int main(int argc, char* argv[]){
  string arg1 = argv[1];
  if(arg1 != "-hex"){
    cout<<"NIJE IZABRANA OPCIJA HEX";return -1;
  }
  //izdvajanje parova ime_sekcije, pocetnaAdresa
  std::vector<std::string> parts;
  int i = 2;
  while(true){
    string s = argv[i];
    if(s.size() > 7){
      if(s.substr(0,7)=="-place="){
        string lastChars = s.substr(7,s.size()-6);      
        std::istringstream iss(lastChars);
        std::string part;

        while (std::getline(iss, part, '@')) {
            parts.push_back(part);
        }
      }else break;
    } else break;
    i++; 
  }
  string nazivIzlaznogFajla = "linkerIzlaz.hex";
  //proveri da li je -o 
  arg1 = argv[i];
  if(arg1 == "-o"){
    i++;
    nazivIzlaznogFajla = argv[i++];
  }
  //napushuj sve fajlove za linkovanje
  std::vector<string> vektorDatoteka;
  while(i < argc){
    vektorDatoteka.push_back(argv[i++]);
  }
  //namestio delove ovog parts i sklopio sec_name poc_adresa 
  std::vector<Placeholder>placeholder; 
  for(int k = 0; k < parts.size(); k+=2){
    Placeholder* p = new Placeholder();
    p->ime_sekcije = parts[k];
    p->hex_adresa  = parts[k+1];
    placeholder.push_back(*p);
  }

  Linker* linker = new Linker();
  linker->ucitaj(vektorDatoteka);
  for(string s: vektorDatoteka)
    linker->datoteke.push_back(s);
  linker->mapiranje(&placeholder);
  linker->odredjivanje();
  linker->razresavanje();
  linker->upis(nazivIzlaznogFajla);
  return 0;
}