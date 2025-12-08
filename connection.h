/**
 * @file connection.h
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для класса соединения
 * @details Определяет класс Connection для управления сетевыми соединениями
 */

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

#define BUFFER_SIZE 1024 ///< Размер буфера для сетевых операций

using namespace std;

/**
 * @class Connection
 * @brief Класс для управления сетевыми соединениями сервера
 * @details Обеспечивает установку соединения, аутентификацию клиентов и обработку данных
 */
class Connection{
public:
    /**
     * @brief Установка соединения и обработка клиентов
     * @param p Указатель на параметры соединения
     * @return Код завершения (0 - успех, 1 - ошибка)
     * @throw std::system_error при ошибках сетевых операций
     */
    static int conn(const Params* p);
};
