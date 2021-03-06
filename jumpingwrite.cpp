#include "mattock-ohash.hpp"
#include <iostream>

template <typename OHash>
std::string jumping() {
  OHash oh(false); //Writing without assuming sparse.
  char buf[4096];
  for (int index=0;index<4096;index++) {
    buf[index]=index%128;
  }
  oh.written_chunk(buf+1024,2048,1024); //Must be ignored
  oh.written_chunk(buf,2048,0); //First half
  oh.written_chunk(buf+1024,2048,1024); //Overlapping chunk, this means a reset.
  oh.written_chunk(buf+1000,1000,1000); //Must be ignored again.
  oh.written_chunk(buf+3000,1096,3000); //Final overlapping chunk, must be ignored.
  oh.written_chunk(buf,2048,0); //First half again.
  oh.written_chunk(buf+3000,1096,3000); //Final chunk, must be ignored again.
  oh.written_chunk(buf+2048,1024,2048); //This should be processed.
  oh.written_chunk(buf+4000,96,4000); //Final overlapping chunk, must be ignored again.
  oh.written_chunk(buf+3072,1024,3072); //This should be processed, we should have everything now.
  oh.done();
  return oh.result();
}; 

int main(int argc,char **argv) {
  bool ok=true;
  std::string legacyhash = jumping<mattock::ohash_legacy>();
  std::string transitionalhash = jumping<mattock::ohash_transitional>();
  std::string fasthash = jumping<mattock::ohash>();
  std::string expectedlegacy="7E34C64D5AEDAB60090857BF54093AF6440001AC";
  std::string expectednew="30E84B7FFDBBA730A3B19D1475D6E389B42BCD26873E3107E9416A9DE408B529";
  std::string expectedtransitional = expectednew + expectedlegacy; 
  if (legacyhash != expectedlegacy) {
    std::cerr << "Invalid legacy hash. Expecting " << expectedlegacy << ", got " << legacyhash << std::endl;
    ok=false;  
  }
  if (fasthash != expectednew) {
    std::cerr << "Invalid fast hash. Expecting " << expectednew << ", got " << fasthash << std::endl;
    ok=false;
  }
  if (transitionalhash != expectedtransitional) {
    std::cerr << "Invalid transitional hash. Expecting " << expectedtransitional << ", got " << transitionalhash << std::endl;
    ok=false;
  }
  if (ok == false) {
    return -1;
  }
  std::cerr << "OK" << std::endl;
  return 0;
}
