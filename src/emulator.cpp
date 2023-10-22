#include "../inc/emulator.hpp"

void Emulator::emuliraj(){

  //44332211  sto znaci da je 11 = OPCODE + MOD 22 = REGA + REGB ...
  //12345693 firstByte = 93; //sto je pop, pa je secondByte = 56 second[0] = regA
  while (true) {
    
    string ceo4Byte = memorija[r15];
    vector<string>result;
    for(int i = 0; i+1 < ceo4Byte.size(); i+=2){
      result.push_back(ceo4Byte.substr(i,2));
    }
    string fourthByte = result[0];

    string thirdByte = result[1];
  
    string secondByte = result[2];
    
    string firstByte = result[3];
    
      long long* gprA = odrediRegistar(secondByte[0]);
      long long* gprB = odrediRegistar(secondByte[1]);
      long long* gprC = odrediRegistar(thirdByte[0]);
      int D = odrediPomeraj(thirdByte, fourthByte);

    //IZVRSI INSTRUKCIJU-------------------------------------------------------
    if(firstByte == "21"){//call
      r14-=4;
      staviUMem(r14, r15);
      r15 = dohvati4ByteIzMemorije(*gprA + *gprB + D);
      continue;
    }else if(firstByte == "38"){//jmp
      r15 = dohvati4ByteIzMemorije(*gprA + D);
      continue;
    }else if(firstByte == "39"){//beq
      
      if(*gprB == *gprC){ r15 = dohvati4ByteIzMemorije(*gprA + D);
        continue;
      }
    
    }else if(firstByte == "3a"){//bne
      if(*gprB != *gprC){ r15 = dohvati4ByteIzMemorije(*gprA + D);
        continue;
      } 
    }else if(firstByte == "3b"){//bgt
      if(*gprB > *gprC){ r15 = dohvati4ByteIzMemorije(*gprA + D);
        continue;
      }
    }else if(firstByte == "91"){//LD %r1, %sp
      *gprA = *gprB + D;   

    }else if(firstByte == "92"){//LD ostali
      *gprA = dohvati4ByteIzMemorije(*gprB + *gprC + D); 
    
    }else if(firstByte =="98"){//LD simbol, r1
      long long medjuRezultat = dohvati4ByteIzMemorije(*gprB + *gprC + D);
      *gprA = dohvati4ByteIzMemorije(medjuRezultat);


    } else if(firstByte == "80"){//ST %r1, value2
      staviUMem(*gprA + *gprB + D , *gprC);

    }else if(firstByte == "82"){//st %r1, [%r2]
      long long medjuRezultat = dohvati4ByteIzMemorije(*gprA + *gprB + D);
      staviUMem(medjuRezultat, *gprC);
      
    }else if(firstByte == "81"){//push
      *gprA = *gprA + D;
      staviUMem(*gprA, *gprC);


    }else if(firstByte == "93"){//pop
      *gprA = dohvati4ByteIzMemorije(*gprB);
      *gprB = *gprB + D;
      

    }else if(firstByte == "60"){//not
      *gprA = ~*gprB;

    }else if(firstByte == "40"){//xchg
      long long temp = *gprB;
      *gprB = *gprC;
      *gprC = temp;
    
    }else if(firstByte == "50"){//add
      *gprA = *gprB + *gprC;

    }else if(firstByte == "51"){//sub
      *gprA = *gprB - *gprC;

    }else if(firstByte == "52"){//mul
      *gprA = *gprB * *gprC;

    }else if(firstByte == "53"){//div
      *gprA = *gprB / *gprC;

    }else if(firstByte == "61"){//and
      *gprA = *gprB & *gprC; 

    }else if(firstByte == "62"){//or
      *gprA = *gprB | *gprC;

    }else if(firstByte == "63"){//xor
      *gprA = *gprB ^ *gprC;

    }else if(firstByte == "70"){//shl
      *gprA = *gprB << *gprC;

    }else if(firstByte == "71"){//shr
      *gprA = *gprB >> *gprC;

    }else if(firstByte == "90"){//csrrd
      if(secondByte[1]=='0'){//status
        *gprA = status;
      }else if(secondByte[1] == '1'){//handler
        *gprA = handler;
      }else if(secondByte[1] == '2'){//cause
        *gprA = cause;
      }

    }else if(firstByte == "94"){//csrwr
      if(secondByte[0]=='0'){//status
        status = *gprB;
      }else if(secondByte[0] == '1'){//handler
        handler = *gprB;
      }else if(secondByte[0] == '2'){//cause
        cause = *gprB;
      }

    }else if(firstByte == "01"){//INT
      r14-=4; 
      staviUMem(r14, status);
      r14-=4;
      staviUMem(r14, r15);
      cause = 4;
      status = status & (~0x1);
      r15 = handler;
      continue;

    }else if(firstByte == "99"){//iret
      //pop pc, pop Status
      r15 = dohvati4ByteIzMemorije(r14);
      r14+=4;
      status = dohvati4ByteIzMemorije(r14);
      r14+=4;
      
    }else if(firstByte == "97"){
      if(secondByte[0]=='0'){//status
        status = dohvati4ByteIzMemorije(*gprB);
      }else if(secondByte[0] == '1'){//handler
        handler = dohvati4ByteIzMemorije(*gprB);
      }else if(secondByte[0] == '2'){//cause
        cause = dohvati4ByteIzMemorije(*gprB);
      }
      *gprB = *gprB + D;

    }else if(firstByte == "00"){//
      pozoviHalt();
      return;

    } else{
      cout<<"NEPOZNATA MASINSKA INSTRUCKIJA U EMULATORU";
      exit(1);
    }
      r15+=4;
  }
}

long long* Emulator::odrediRegistar(char bitoviReg){
  switch (bitoviReg)
  {
  case '0': return &r0;
  case '1': return &r1;
  case '2': return &r2;
  case '3': return &r3;
  case '4': return &r4;
  case '5': return &r5;
  case '6': return &r6;
  case '7': return &r7;
  case '8': return &r8;
  case '9': return &r9;
  case 'a': return &r10;
  case 'b': return &r11;
  case 'c': return &r12;
  case 'd': return &r13;
  case 'e': return &r14;
  case 'f': return &r15;
  default:
    break;
  }
  cout<<"GRESKA PRI ODREDJIVANJU REGISTRA";
  exit(1);
}

//mem32[vr] <= gpr
void Emulator::staviUMem(long long vr, long long gpr){
  
  string hexString = intToHexString(gpr);
  hexString = dopuniString(hexString);
   string pom = "";
  for(int i = hexString.size()-1; i > 0; i-=2){
    int j = i-1;
    pom += hexString[j];
    pom+= hexString[i];
  }
  memorija[vr] = pom;
    
}

int Emulator::odrediPomeraj(string thirdByte,string fourthByte){

  string pom ="";
  pom += thirdByte[1] +fourthByte;
  int intValue;
  // Convert the hex string to an integer using std::stringstream
    std::stringstream ss;
    ss << std::hex << pom;
    ss >> intValue;

    // Handle the sign bit for a 12-bit signed integer
    if (intValue > 2047) {
        // Convert the value to its two's complement representation
        intValue = intValue - 4096;
    }
  return intValue;
}

//mem32[vr]
long long Emulator::dohvati4ByteIzMemorije(long long vr){
  string hexString2 = memorija[vr];
  string hexString = "";
  for(int i = 7; i > 0; i-=2){
    hexString += hexString2[i-1];
    hexString += hexString2[i];
  }
  long long intValue = std::stoll(hexString, nullptr, 16);
  return intValue;
}


string Emulator::intToHexString(long long value){
  std::stringstream stream;
  stream << std::hex << value;  // Convert integer to hexadecimal string
  std::string hexString = stream.str();  // Get the hexadecimal string
  return hexString;
}

string Emulator::dopuniString (string hex){
    string povratni;
    switch (hex.size())
    {
    case 1:povratni ="0000000" + hex;
      /* code */
      break;
    case 2:povratni ="000000" + hex;
      /* code */
      break;
    case 3:povratni ="00000" + hex;
      /* code */
      break;
    case 4:povratni ="0000" + hex;
      /* code */
      break;
    case 5:povratni="000" + hex;
      /* code */
      break;
    case 6:povratni="00" + hex;
      /* code */
      break;
    case 7:povratni = "0" + hex;
      /* code */
      break;
    case 8:povratni = hex;
      break;
    default:
      break;
    }
    if(hex.size() > 8) povratni = hex.substr(hex.size() - 8);
    return povratni;
}

void Emulator::pozoviHalt(){
  cout<<"Emulated processor executed halt instruction\n";
  cout<<"Emulated processor state:\n";
  cout << "r0=0x"<<dopuniString(intToHexString(r0))<<"\t"<<"r1=0x"<<dopuniString (intToHexString(r1))<<"\t"<<"r2=0x"<<dopuniString (intToHexString(r2))<< "\t"<<"r3=0x"<<dopuniString (intToHexString(r3))<<std::endl;
  cout << "r4=0x"<<dopuniString (intToHexString(r4))<<"\t"<<"r5=0x"<<dopuniString (intToHexString(r5))<<"\t"<<"r6=0x"<<dopuniString (intToHexString(r6))<< "\t"<<"r7=0x"<<dopuniString(intToHexString(r7))<<std::endl;
  cout << "r8=0x"<<dopuniString (intToHexString(r8))<<"\t"<<"r9=0x"<<dopuniString (intToHexString(r9))<<"\t"<<"r10=0x"<<dopuniString (intToHexString(r10))<< "\t"<<"r11=0x"<<dopuniString( intToHexString(r11))<<std::endl;
  cout << "r12=0x"<<dopuniString (intToHexString(r12))<<"\t"<<"r13=0x"<<dopuniString (intToHexString(r13))<<"\t"<<"r14=0x"<<dopuniString (intToHexString(r14))<< "\t"<<"r15=0x"<<dopuniString (intToHexString(r15))<<std::endl;
}

void Emulator::ucitaj_memoriju(string naziv_datoteke){

  std::ifstream inputFile(naziv_datoteke); 
  if (!inputFile.is_open()) {
    std::cerr << "Error opening the file." << std::endl;
  }

  string line;
  while(std::getline(inputFile, line)){
    if(line == "" || line.size() == 0 || line == " " || line == "\n")continue;
    
    std::vector<std::string> substrings;

    // Use std::istringstream to split the string
    std::istringstream iss(line);
    std::string token;
    //sad u substrings imamo [0]= "adresa:" [1]= "11223344" 
    while (std::getline(iss, token, ' ')) {
        substrings.push_back(token);
    }
    //obrisi : kod adrese
    substrings[0] = substrings[0].erase(substrings[0].size()-1);

    long long adresa = stoll (substrings[0],nullptr, 16);
    
    memorija[adresa] = substrings[1];
    
  }

  inputFile.close();

}

int main(int argc, char* argv[]){

  if(argc!= 2){
    cout<< "NE VALJA BROJ ARGUMENATA PROSLEDJEN U KOMANDNOJ LINIJI";
    return -1;
  }
  string inputFile = argv [1];

  Emulator* em = new Emulator();
  em->ucitaj_memoriju(inputFile);
  em->emuliraj();

  return 0;
}