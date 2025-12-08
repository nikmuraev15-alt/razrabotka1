/**
 * @file crypto.h
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Заголовочный файл для криптографических функций
 * @details Определяет функцию аутентификации с использованием CryptoPP
 */

#pragma once
#include <string>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

using namespace std;
namespace CPP = CryptoPP;

/**
 * @brief Функция аутентификации
 * @param salt Соль для хеширования
 * @param pass Пароль пользователя
 * @return Хеш SHA-256 от соли и пароля
 */
string auth(string salt, string pass);
