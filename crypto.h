#pragma onces
#include <string>
#include <string>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
using namespace std;
namespace CPP = CryptoPP;
string auth(string salt, string pass);