#pragma once

struct TXRecord {
   const char* hash;
   const char* trunk;
   const char* branch;
};

//empty hash in TXRecord means it was not found
TXRecord findInTangleDB(const  char* hash);

void putInTangleDB(const char* hash,TXRecord tx);





