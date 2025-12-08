/**
 * @file log.h
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для функций логирования
 * @details Определяет функции для работы с системой логирования
 */

#include "connection.h"
#include "interface.h"
#include <fstream>
#include <system_error>
#include <chrono>
#include <iomanip>

/**
 * @brief Получение текущего времени в формате строки
 * @return Строка с текущим временем в формате "ГГГГ-ММ-ДД ЧЧ:ММ:СС.мс"
 */
std::string getCurrentTime();

/**
 * @brief Запись ошибки в лог-файл
 * @param logFile Имя файла лога
 * @param errorMessage Сообщение об ошибке
 */
void logError(const std::string& logFile, const std::string& errorMessage);
