#include "connection.h"
#include "log.cpp"

int Connection::conn(const Params* p){
    
    
    int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == -1){       
            std::string errorMsg = "Ошибка создания сокета: " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            throw std::system_error(errno, std::generic_category()); 
        }


        std::unique_ptr <sockaddr_in> self_addr(new sockaddr_in); 
        self_addr->sin_family = AF_INET; // всегда так
        self_addr->sin_port = htons(p->Port); //Порт
        self_addr->sin_addr.s_addr = inet_addr(p->Address.c_str()); // автовыбор

    int rc = bind(s, reinterpret_cast<const sockaddr*>(self_addr.get()), sizeof(sockaddr_in));
    if (rc == -1) {
        std::string errorMsg = "Ошибка bind: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }


    rc = listen(s, 5);
    if (rc == -1) {
        std::string errorMsg = "Ошибка listen: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }


    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(s, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    if (client_socket == -1) {
        std::string errorMsg = "Ошибка accept: " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }

    char buffer[BUFFER_SIZE];
    ssize_t received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes == -1){
        std::string errorMsg = "Ошибка recv (логин): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }


    if (received_bytes > 0) {
        buffer[received_bytes] = '\0'; // Добавляем нулевой терминатор
        //std::cout << "Получено сообщение от клиента: " << buffer << std::endl;
    }

    ifstream dataFile(p->inFileName);
    string login;
    dataFile >> login;
    string pass;
    dataFile >> pass;
    
    //string login(buffer);
    string salt = "0000000000000000";
    string message = salt;
    ssize_t sent_bytes = send(client_socket, message.c_str(), message.length(), 0);
    if (!dataFile.is_open()) {
        std::string errorMsg = "Не удалось открыть файл данных: " + string(p->inFileName);
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::runtime_error(errorMsg);
    }

    received_bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes == -1) throw std::system_error(errno, std::generic_category());
    
    if (received_bytes > 0) {
        buffer[received_bytes] = '\0'; // Добавляем нулевой терминатор
        std::cout << "Получено сообщение от клиента: " << buffer << std::endl;
    }


    string hash = auth(salt, pass);

    if(buffer == hash){
        message = "OK";
    } else {
        message = "ERR";
        logError(p->logFile, "Ошибка аутентификации: неверный хеш");
    }
    sent_bytes = send(client_socket, message.c_str(), message.length(), 0);
    if (sent_bytes == -1) {
        std::string errorMsg = "Ошибка send (результат аутентификации): " + std::string(strerror(errno));
        logError(p->logFile, errorMsg);
        close(client_socket);
        close(s);
        throw std::system_error(errno, std::generic_category());
    }
    //cout << "Мой хеш: " << hash << std::endl;

    //recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    //if (received_bytes == -1) throw std::system_error(errno, std::generic_category());

    uint32_t vectors_count;
    ssize_t total_received = 0;
    char* buff = reinterpret_cast<char*>(&vectors_count);


    // 1. Получаем количество векторов
    while (total_received < sizeof(vectors_count)) {
        ssize_t received = recv(client_socket, buff + total_received, sizeof(vectors_count) - total_received, 0);
        if (received <= 0) {
            std::string errorMsg = "Ошибка recv (количество векторов): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            close(client_socket);
            close(s);
            throw std::system_error(errno, std::generic_category());
        }
        total_received += received;
    }
    cout << "Количество векторов: " << vectors_count << endl;

    // Обрабатываем каждый вектор
    for (uint32_t vector_idx = 0; vector_idx < vectors_count; vector_idx++) {
        // 2. Получаем длину текущего вектора
        uint32_t vector_size;
        total_received = 0;
        buff = reinterpret_cast<char*>(&vector_size);
        
        while (total_received < sizeof(vector_size)) {
            ssize_t received = recv(client_socket, buff + total_received, sizeof(vector_size) - total_received, 0);
            if (received <= 0) {
                std::string errorMsg = "Ошибка recv (размер вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
                logError(p->logFile, errorMsg);
                close(client_socket);
                close(s);
                throw std::system_error(errno, std::generic_category());
            }
            total_received += received;
        }
        cout << "Вектор " << vector_idx << ", длина: " << vector_size << endl;

        // 3. Получаем элементы текущего вектора
        uint32_t result = 1;  // Перенес объявление сюда, чтобы считать произведение для каждого вектора
        bool overflow = false;
        for (uint32_t elem_idx = 0; elem_idx < vector_size; elem_idx++) {
            uint16_t element;
            total_received = 0;
            buff = reinterpret_cast<char*>(&element);
            
            while (total_received < sizeof(element)) {
                ssize_t received = recv(client_socket, buff + total_received, sizeof(element) - total_received, 0);
                if (received <= 0) {
                    std::string errorMsg = "Ошибка recv (элемент " + std::to_string(elem_idx) + " вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
                    logError(p->logFile, errorMsg);
                    close(client_socket);
                    close(s);
                    throw std::system_error(errno, std::generic_category());
                }total_received += received;
            }
            cout << "  Элемент " << elem_idx << ": " << element << endl;
            result = result * element;
        }
        cout << result << endl;
        if(result > 65535) result = 65535;
        if(result < 0) result = 0;
        //resultFile << result << endl;
        ssize_t send_result = send(client_socket, &result, sizeof(result), 0);
        if (send_result == -1) {
            std::string errorMsg = "Ошибка send (результат вектора " + std::to_string(vector_idx) + "): " + std::string(strerror(errno));
            logError(p->logFile, errorMsg);
            close(client_socket);
            close(s);
            throw std::system_error(errno, std::generic_category());
        }
        close(s);
    }
    
    close(client_socket);
    return 0;
};



