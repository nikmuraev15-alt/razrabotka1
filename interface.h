/**
 * @file interface.h
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для пользовательского интерфейса
 * @details Определяет структуру параметров и класс для обработки аргументов командной строки
 */

#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <sstream>

using namespace std;
namespace po = boost::program_options;

/**
 * @struct Params
 * @brief Структура для хранения параметров программы
 */
struct Params {
    string inFileName;      ///< Имя файла с пользователями
    string inFileJournal;   ///< Имя журнального файла
    string inFileData;      ///< Имя файла данных
    string logFile;         ///< Имя файла лога
    int Port;               ///< Порт сервера
    string Address;         ///< Адрес сервера
};

/**
 * @class UserInterface
 * @brief Класс для обработки аргументов командной строки
 * @details Использует boost::program_options для парсинга параметров
 */
class UserInterface {
private:
    po::options_description desc;   ///< Описание параметров командной строки
    po::variables_map vm;           ///< Переменные после парсинга
    Params params;                  ///< Параметры программы

public:
    /**
     * @brief Конструктор по умолчанию
     */
    UserInterface();
    
    /**
     * @brief Парсинг аргументов командной строки
     * @param argc Количество аргументов
     * @param argv Массив аргументов
     * @return true если парсинг успешен, false если требуется показать справку
     */
    bool Parser(int argc, const char** argv);
    
    /**
     * @brief Получение описания параметров
     * @return Строка с описанием параметров
     */
    string getDescription();
    
    /**
     * @brief Получение параметров программы
     * @return Структура с параметрами
     */
    Params getParams() {
        return params;
    };
};
