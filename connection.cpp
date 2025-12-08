/**
 * @file connection.cpp
 * @author Мураев Н.Д.
 * @version 1.0
 * @date 2025
 * @copyright ИБСТ ПГУ
 * @brief Реализация сетевого соединения сервера
 * @details Содержит функции для установки соединения, аутентификации и обработки данных от клиентов
 */

#include "connection.h"
#include "log.h"
#include <fstream>
#include <vector>
#include <algorithm>

/**
 * @brief Поиск пользователя в файле по логину с кэшированием
 * @param filename Имя файла с пользователями
 * @param username Логин для поиска
 * @param password Найденный пароль (выходной параметр)
 * @return true если пользователь найден, false если нет
 * @warning Файл пользователей загружается только при первом вызове
 */
bool findUserInFile(const std::string& filename, const std::string& username, std::string& password) {
    static std::vector<std::pair<std::string, std::string>> userCache;
    static bool cacheLoaded = false;
    
    // Загружаем кэш при первом вызове
    if (!cacheLoaded) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            size_t pos = line.find(':');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string file_user = line.substr(0, pos);
            std::string file_pass = line.substr(pos + 1);
            
            // Убираем пробелы
            file_user.erase(0, file_user.find_first_not_of(" \t"));
            file_user.erase(file_user.find_last_not_of(" \t") + 1);
            file_pass.erase(0, file_pass.find_first_not_of(" \t"));
            file_pass.erase(file_pass.find_last_not_of(" \t") + 1);
            
            userCache.emplace_back(file_user, file_pass);
        }
        cacheLoaded = true;
        file.close();
    }
    
    // Ищем в кэше
    auto it = std::find_if(userCache.begin(), userCache.end(),
        [&username](const auto& pair) { return pair.first == username; });
    
    if (it != userCache.end()) {
        password = it->second;
        return true;
    }
    
    return false;
}

/**
 * @brief Генерация случайной соли
 * @param length Длина соли (по умолчанию 16)
 * @return Случайная соль
 */
std::string generateSalt(size_t length = 16) {
    static const char charset[] = 
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::string salt;
    salt.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        salt += charset[rand() % (sizeof(charset) - 1)];
    }
    
    return salt;
}

/**
 * @brief Обработка одного вектора с проверкой на переполнение
 * @param client_socket Сокет клиента
 * @param vector_size Размер вектора
 * @param p Параметры соединения
 * @return Результат обработки вектора (произведение элементов)
 * @throw std::system_error при ошибках сетевых операций
 * @warning Проверяет переполнение и ограничивает размер вектора
 */
uint32_t processVector(int client_socket, uint32_t vector_size, const Params* p) {
    if (vector_size == 0) {
        return 1; // Произведение пустого вектора = 1
    }
    
    if (vector_size > 10000) { // Защита от слишком больших векторов
        std::string errorMsg = "Слишком большой размер вектора: " + std::to_string(vector_size);
        logError(p->logFile, errorMsg);
        return 0;
    }
    
    uint64_t result = 1; // Используем 64-бит для предотвращения переполнения
    
    for (uint32_t elem_idx = 0; elem_idx < vector_size; elem_idx++) {
        uint16_t element;
        ssize_t total_received = 0;
        char* buff = reinterpret_cast<char*>(&element);
        
        while (total_received < sizeof(element)) {
            ssize_t received = recv(client_socket, buff + total_received, 
                                   sizeof(element) - total_received, 0);
            if (received <= 0) {
                std::string errorMsg = "Ошибка recv (элемент вектора): " + std::string(strerror(errno));
                logError(p->logFile, errorMsg);
                throw std::system_error(errno, std::generic_category());
            }
            total_received += received;
        }
        
        // Проверка на переполнение
        if (element != 0 && result > UINT32_MAX / element) {
            logError(p->logFile, "Обнаружено переполнение при умножении вектора");
            return UINT32_MAX; // Максимальное значение при переполнении
        }
        
        result *= element;
    }
    
    return static_cast<uint32_t>(std::min(result, static_cast<uint64_t>(UINT32_MAX)));
}

/**
 * @brief Безопасное получение данных фиксированного размера
 * @param socket Сокет
 * @param buffer Буфер для данных
 * @param size Размер данных
 * @param p Параметры соединения
 * @param context Контекст для сообщения об ошибке
 * @throw std::system_error при ошибках получения данных
 */
void safeRecv(int socket, void* buffer, size_t size, const Params* p, const std::string& context) {
    ssize_t total_received = 0;
    char* buff = reinterpret_cast<char*>(buffer);
    
    while (total_received < size) {
        ssize_t received = recv(socket, buff + total_received, size - total_received, 0);
        if (received <= 0) {
            std::string errorMsg = "Ошибка recv (" + context + "): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            throw std::system_error(errno, std::generic_category());
        }
        total_received += received;
    }
}

/**
 * @brief Безопасная отправка данных
 * @param socket Сокет
 * @param data Данные для отправки
 * @param size Размер данных
 * @param p Параметры соединения
 * @param context Контекст для сообщения об ошибке
 * @throw std::system_error при ошибках отправки данных
 */
void safeSend(int socket, const void* data, size_t size, const Params* p, const std::string& context) {
    ssize_t sent_bytes = send(socket, data, size, 0);
    if (sent_bytes == -1) {
        std::string errorMsg = "Ошибка send (" + context + "): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        throw std::system_error(errno, std::generic_category());
    }
}

/**
 * @brief Основной метод установки соединения и обработки клиентов
 * @param p Указатель на параметры соединения
 * @return Код завершения (0 - успех, 1 - ошибка)
 * @throw std::system_error при ошибках сетевых операций
 * @details Выполняет полный цикл работы сервера: создание сокета, привязка, 
 * прослушивание, аутентификация клиента и обработка данных
 */
int Connection::conn(const Params* p) {
    // Инициализация генератора случайных чисел для соли
    srand(static_cast<unsigned int>(time(nullptr)));
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {       
        std::string errorMsg = "Ошибка создания сокета: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        throw std::system_error(errno, std::generic_category()); 
    }

    // Устанавливаем опцию повторного использования адреса
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::string errorMsg = "Ошибка setsockopt: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
    }

    std::unique_ptr<sockaddr_in> self_addr(new sockaddr_in); 
    self_addr->sin_family = AF_INET;
    self_addr->sin_port = htons(p->Port);
    self_addr->sin_addr.s_addr = inet_addr(p->Address.c_str());

    int rc = bind(server_socket, reinterpret_cast<const sockaddr*>(self_addr.get()), sizeof(sockaddr_in));
    if (rc == -1) {
        std::string errorMsg = "Ошибка bind: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(server_socket);
        throw std::system_error(errno, std::generic_category());
    }

    rc = listen(server_socket, 10); // Увеличили очередь подключений
    if (rc == -1) {
        std::string errorMsg = "Ошибка listen: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(server_socket);
        throw std::system_error(errno, std::generic_category());
    }

    // Логируем запуск сервера
    std::string startMsg = "Сервер запущен на " + p->Address + ":" + std::to_string(p->Port);
    logError(p->logFile, startMsg);

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_socket == -1) {
        std::string errorMsg = "Ошибка accept: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(server_socket);
        throw std::system_error(errno, std::generic_category());
    }

    // Логируем подключение клиента
    std::string connectMsg = "Клиент подключен: " + std::string(inet_ntoa(client_addr.sin_addr));
    logError(p->logFile, connectMsg);

    try {
        // Получаем логин от клиента
        char buffer[BUFFER_SIZE];
        ssize_t received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (received_bytes == -1) {
            std::string errorMsg = "Ошибка recv (логин): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            throw std::system_error(errno, std::generic_category());
        }

        if (received_bytes > 0) {
            buffer[received_bytes] = '\0';
        }

        std::string client_login(buffer);
        
        // Ищем пользователя в файле
        std::string user_password;
        if (!findUserInFile(p->inFileName, client_login, user_password)) {
            std::string errorMsg = "Пользователь не найден: " + client_login;
            logError(p->logFile, errorMsg);
            
            std::string message = "ERR_USER_NOT_FOUND";
            safeSend(client_socket, message.c_str(), message.length(), p, "ошибка пользователя");
            
            close(client_socket);
            close(server_socket);
            return 1;
        }

        // Генерируем и отправляем случайную соль
        std::string salt = generateSalt();
        safeSend(client_socket, salt.c_str(), salt.length(), p, "соль");

        // Получаем хеш от клиента
        received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (received_bytes == -1) {
            std::string errorMsg = "Ошибка recv (хеш): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            throw std::system_error(errno, std::generic_category());
        }
        
        if (received_bytes > 0) {
            buffer[received_bytes] = '\0';
        }

        // Проверяем хеш
        std::string computed_hash = auth(salt, user_password);
        std::string response;
        
        if (std::string(buffer) == computed_hash) {
            response = "OK";
            logError(p->logFile, "Аутентификация успешна для пользователя: " + client_login);
        } else {
            response = "ERR_AUTH_FAILED";
            logError(p->logFile, "Ошибка аутентификации: неверный хеш для пользователя: " + client_login);
        }
        
        safeSend(client_socket, response.c_str(), response.length(), p, "результат аутентификации");

        if (response != "OK") {
            close(client_socket);
            close(server_socket);
            return 1;
        }

        // Получаем количество векторов
        uint32_t vectors_count;
        safeRecv(client_socket, &vectors_count, sizeof(vectors_count), p, "количество векторов");
        
        // Проверяем разумность количества векторов
        if (vectors_count > 1000) {
            logError(p->logFile, "Слишком большое количество векторов: " + std::to_string(vectors_count));
            vectors_count = 1000; // Ограничиваем
        }

        // Обрабатываем каждый вектор
        for (uint32_t vector_idx = 0; vector_idx < vectors_count; vector_idx++) {
            uint32_t vector_size;
            safeRecv(client_socket, &vector_size, sizeof(vector_size), p, "размер вектора");

            uint32_t result = processVector(client_socket, vector_size, p);
            
            safeSend(client_socket, &result, sizeof(result), p, "результат вектора");
        }

        logError(p->logFile, "Обработка завершена успешно");

    } catch (const std::exception& e) {
        std::string errorMsg = "Исключение в обработке клиента: " + std::string(e.what());
        logError(p->logFile, errorMsg);
    }

    close(client_socket);
    close(server_socket);
    return 0;
}
