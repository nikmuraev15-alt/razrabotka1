#include "crypto.h"

string auth(string salt, string pass){

CPP::SHA256 sha256;
string hash;
CPP::StringSource(
    string(salt).append(pass), 
    true,
        new CPP::HashFilter(
            sha256,
            new CPP::HexEncoder(
                new CPP::StringSink(hash)))
);

return hash;
}
