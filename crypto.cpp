/**
 * @file crypto.cpp
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация криптографических функций
 * @details Содержит функцию для вычисления хеша аутентификации
 */

#include "crypto.h"

/**
 * @brief Вычисление хеша аутентификации
 * @param salt Соль для хеширования
 * @param pass Пароль пользователя
 * @return Хеш SHA-256 в hex-формате
 * @details Использует алгоритм SHA-256 из библиотеки CryptoPP
 */
string auth(string salt, string pass){
    CPP::SHA256 sha256;
    string hash;
    CPP::StringSource(
        string(salt).append(pass), 
        true,
            new CPP::HashFilter(
                sha256,
                new CPP::HexEncoder(
                    new CPP::StringSink(hash)))
    );

    return hash;
}
