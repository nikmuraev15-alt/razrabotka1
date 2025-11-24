#pragma once
#include "errno.h"
#include "crypto.h"
#include "interface.h"
#include <system_error>
#include <netinet/in.h>
#include <memory>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#define BUFFER_SIZE 1024
using namespace std;
class Connection{
    public:
int conn(const Params* p);
};