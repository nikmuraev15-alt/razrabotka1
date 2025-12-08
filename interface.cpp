/**
 * @file interface.cpp
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация пользовательского интерфейса
 * @details Содержит функции для парсинга и обработки аргументов командной строки
 */

#include "interface.h"

/**
 * @brief Конструктор UserInterface
 * @details Инициализирует описание параметров командной строки
 */
UserInterface::UserInterface() : desc("Allowed options")
{
    // добавление параметров в парсер командной строки
    desc.add_options()
    ("help,h", "Show help")
    ("log,l", po::value<string>(&params.logFile)->default_value("journal.txt"), "Set log file")
    ("base,b", po::value<std::string>(&params.inFileName)->required(),"Set input data base name")
    ("journal,j", po::value<std::string>(&params.inFileJournal)->required(),"Set journal file name")
    ("port,p", po::value<int>(&params.Port)->required(), "Set port")
    ("address,a", po::value<string>(&params.Address)->default_value("127.0.0.1"), "Set address");
}

/**
 * @brief Парсинг аргументов командной строки
 * @param argc Количество аргументов
 * @param argv Массив аргументов
 * @return true если парсинг успешен, false если требуется показать справку
 */
bool UserInterface::Parser(int argc, const char** argv)
{
    // Если нет аргументов или только --help, показываем справку
    if (argc == 1) {
        return false;
    }
    // парсинг
    po::store(po::parse_command_line(argc, argv, desc), vm);
    // обрабатываем --help до вызова notify
    if (vm.count("help"))
    return false;
    // присвоение значений по умолчанию и возбуждение исключений
    po::notify(vm);
    return true;
}

/**
 * @brief Получение описания параметров
 * @return Строка с описанием параметров
 */
std::string UserInterface::getDescription()
{
    std::ostringstream ss;
    ss << desc;
    return ss.str();
}
