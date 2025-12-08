/**
 * @file log.cpp
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация функций логирования
 * @details Содержит функции для записи логов с временными метками
 */

#include "log.h"

/**
 * @brief Получение текущего времени в формате строки
 * @return Строка с текущим временем в формате "ГГГГ-ММ-ДД ЧЧ:ММ:СС.мс"
 */
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

/**
 * @brief Запись ошибки в лог-файл
 * @param logFile Имя файла лога
 * @param errorMessage Сообщение об ошибке
 * @details Добавляет временную метку и записывает сообщение в файл
 */
void logError(const std::string& logFile, const std::string& errorMessage) {
    std::ofstream logStream(logFile, std::ios::app);
    if (logStream.is_open()) {
        logStream << "[" << getCurrentTime() << "] ERROR: " << errorMessage << std::endl;
        logStream.close();
    }
}
