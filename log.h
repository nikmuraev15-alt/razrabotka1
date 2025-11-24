#include "connection.h"
#include "interface.h"
#include <fstream>
#include <system_error>
#include <chrono>
#include <iomanip>
std::string getCurrentTime();
void logError(const std::string& logFile, const std::string& errorMessage);