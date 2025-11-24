#include "connection.h"
#include "interface.h"

int main(int argc, const char** argv)
{
    UserInterface interface;
        if(!interface.Parser(argc, argv)){
            cout << interface.getDescription() << endl;
            return 1;
        }
    Params params = interface.getParams();
    Connection::conn(&params);//&params
    return 0;
}