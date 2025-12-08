/**
 * @file main.cpp
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Главный модуль серверного приложения
 * @details Основная точка входа в программу, обрабатывающая аргументы командной строки и запускающая сервер
 */

#include "connection.h"
#include "interface.h"

/**
 * @brief Главная функция программы
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения программы (0 - успех, 1 - ошибка)
 * @warning Для работы программы необходимо указать обязательные параметры
 */
int main(int argc, const char** argv)
{
    UserInterface userinterface;
    if(!userinterface.Parser(argc, argv)){
        cout << userinterface.getDescription() << endl;
        return 1;
    }
    Params params = userinterface.getParams();
    Connection::conn(&params);
    return 0;
}
