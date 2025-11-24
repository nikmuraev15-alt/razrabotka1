#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <sstream>
using namespace std;
namespace po = boost::program_options;
// структура для хранения полученных параметров
struct Params {
    string inFileName;
    string inFileJournal;
    string inFileData;
    string logFile;
    int Port;
    string Address;
};

// Интерфейс
class Interface {
private:
    // пустой парсер
    po::options_description desc;
    // переменная для результатов парсинга
    po::variables_map vm;
    // результаты парсинга параметров
    Params params;
public:
    Interface();
    bool Parser(int argc, const char** argv);
    string getDescription();
    Params getParams() {
    return params;
    };
};