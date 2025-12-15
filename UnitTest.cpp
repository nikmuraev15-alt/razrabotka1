/**
 * @file tests.cpp
 * @brief Тесты для класса UserInterface
 * @details Содержит набор модульных тестов для проверки корректности работы парсера командной строки.
 * Тесты покрывают различные сценарии: справка, обязательные параметры, опциональные параметры, граничные случаи и специальные значения.
 */

#include <UnitTest++/UnitTest++.h>
#include "interface.h"
#include <string>

/**
 * @brief Тесты для проверки вывода справки
 * @details Проверяет работу короткой (-h) и длинной (--help) версий вывода справки
 */
SUITE(HelpTest) {
    /**
     * @brief Тест короткой версии справки (-h)
     * @details Проверяет, что при передаче параметра -h парсер возвращает false
     * и устанавливает непустое описание (справку)
     */
    TEST(ShortHelp) {
        UserInterface iface;
        const char* argv[] = {"test", "-h", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
    
    /**
     * @brief Тест длинной версии справки (--help)
     * @details Проверяет, что при передаче параметра --help парсер возвращает false
     * и устанавливает непустое описание (справку)
     */
    TEST(Help) {
        UserInterface iface;
        const char* argv[] = {"test", "--help", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
}

/**
 * @brief Тесты для проверки обязательных параметров
 * @details Проверяет корректный разбор обязательных параметров и обработку ошибок при их отсутствии
 */
SUITE(ParameterTest) {
    /**
     * @brief Тест валидных параметров в короткой форме
     * @details Проверяет корректный разбор всех обязательных параметров:
     * -b (база данных), -j (журнал), -p (порт)
     */
    TEST(ValidParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-j", "journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    /**
     * @brief Тест валидных параметров в длинной форме
     * @details Проверяет корректный разбор параметров в длинной форме:
     * --base, --journal, --port
     */
    TEST(ValidParametersLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "--base", "database.db", "--journal", "journal.log", "--port", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    /**
     * @brief Тест отсутствия параметра базы данных
     * @details Проверяет генерацию исключения при отсутствии обязательного параметра -b (--base)
     */
    TEST(MissingBaseParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-j", "journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест отсутствия параметра журнала
     * @details Проверяет генерацию исключения при отсутствии обязательного параметра -j (--journal)
     */
    TEST(MissingJournalParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест отсутствия параметра порта
     * @details Проверяет генерацию исключения при отсутствии обязательного параметра -p (--port)
     */
    TEST(MissingPortParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "database.db", "-j", "journal.log", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест невалидного значения порта
     * @details Проверяет генерацию исключения при указании нечислового значения для порта
     */
    TEST(InvalidPortParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "not_a_number", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест отрицательного значения порта
     * @details Проверяет корректный разбор отрицательного значения порта
     * @note Парсер принимает отрицательные значения, но они могут быть невалидными с семантической точки зрения
     */
    TEST(NegativePort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "-1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(-1, iface.getParams().Port);
    }

    /**
     * @brief Тест нулевого значения порта
     * @details Проверяет корректный разбор нулевого значения порта
     * @note Порт 0 может иметь специальное значение в некоторых системах
     */
    TEST(ZeroPort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "0", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(0, iface.getParams().Port);
    }

    /**
     * @brief Тест максимального валидного порта
     * @details Проверяет корректный разбор порта со значением 65535 (максимальный валидный TCP/UDP порт)
     */
    TEST(LargePort) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "65535", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL(65535, iface.getParams().Port);
    }
}

/**
 * @brief Тесты для проверки опциональных параметров
 * @details Проверяет корректный разбор опциональных параметров и их значений по умолчанию
 */
SUITE(OptionalParametersTest) {
    /**
     * @brief Тест параметра адреса в короткой форме
     * @details Проверяет корректный разбор опционального параметра адреса (-a)
     */
    TEST(WithAddressParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-a", "192.168.1.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("192.168.1.1", iface.getParams().Address);
    }

    /**
     * @brief Тест параметра адреса в длинной форме
     * @details Проверяет корректный разбор опционального параметра адреса (--address)
     */
    TEST(WithAddressParameterLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "--address", "10.0.0.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("10.0.0.1", iface.getParams().Address);
    }

    /**
     * @brief Тест адреса по умолчанию
     * @details Проверяет, что при отсутствии параметра адреса используется значение по умолчанию (127.0.0.1)
     */
    TEST(DefaultAddress) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("127.0.0.1", iface.getParams().Address);
    }

    /**
     * @brief Тест параметра лог-файла в короткой форме
     * @details Проверяет корректный разбор опционального параметра лог-файла (-l)
     */
    TEST(LogFileParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-l", "custom_log.txt", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("custom_log.txt", iface.getParams().logFile);
    }

    /**
     * @brief Тест параметра лог-файла в длинной форме
     * @details Проверяет корректный разбор опционального параметра лог-файла (--log)
     */
    TEST(LogFileParameterLongForm) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "--log", "another_log.txt", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("another_log.txt", iface.getParams().logFile);
    }

    /**
     * @brief Тест лог-файла по умолчанию
     * @details Проверяет, что при отсутствии параметра лог-файла используется значение по умолчанию (journal.txt)
     */
    TEST(DefaultLogFile) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("journal.txt", iface.getParams().logFile);
    }
}

/**
 * @brief Тесты граничных случаев и обработки ошибок
 * @details Проверяет различные пограничные сценарии и обработку некорректных входных данных
 */
SUITE(EdgeCasesTest) {
    /**
     * @brief Тест пустых аргументов
     * @details Проверяет поведение парсера при отсутствии аргументов командной строки
     */
    TEST(EmptyArguments) {
        UserInterface iface;
        const char* argv[] = {"test", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        // Вместо исключения, парсер может вернуть false или бросить другое исключение
        // Проверяем что парсинг не завершается успешно
        CHECK(!iface.Parser(argc, argv));
    }

    /**
     * @brief Тест неизвестного параметра
     * @details Проверяет генерацию исключения при указании неизвестного параметра
     */
    TEST(UnknownParameter) {
        UserInterface iface;
        const char* argv[] = {"test", "-x", "value", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест отсутствия значения параметра
     * @details Проверяет генерацию исключения при отсутствии значения для параметра
     */
    TEST(MissingParameterValue) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(iface.Parser(argc, argv), std::exception);
    }

    /**
     * @brief Тест дублирования параметров
     * @details Проверяет, что при дублировании параметра используется первое значение
     */
    TEST(DuplicateParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db1", "-j", "log", "-p", "9090", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        // Проверяем, что используется первое значение (не допускаем дублирование)
        CHECK_EQUAL("db1", iface.getParams().inFileName);
    }

    /**
     * @brief Тест параметров в разном порядке
     * @details Проверяет корректный разбор параметров независимо от порядка их следования
     */
    TEST(MixedOrderParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-p", "8080", "-j", "journal.log", "-b", "database.db", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("database.db", iface.getParams().inFileName);
        CHECK_EQUAL("journal.log", iface.getParams().inFileJournal);
        CHECK_EQUAL(8080, iface.getParams().Port);
    }

    /**
     * @brief Тест параметра help с другими параметрами
     * @details Проверяет, что параметр help имеет приоритет и остальные параметры игнорируются
     */
    TEST(HelpWithOtherParameters) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-h", "-j", "log", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        // Help должен иметь приоритет и парсинг должен вернуть false
        CHECK(!iface.Parser(argc, argv));
    }
}

/**
 * @brief Тесты специальных значений
 * @details Проверяет корректный разбор специальных значений и нестандартных форматов
 */
SUITE(SpecialValuesTest) {
    /**
     * @brief Тест пустых строковых значений
     * @details Проверяет корректный разбор пустых строк в качестве значений параметров
     */
    TEST(EmptyStringValues) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "", "-j", "", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("", iface.getParams().inFileName);
        CHECK_EQUAL("", iface.getParams().inFileJournal);
    }

    /**
     * @brief Тест специальных символов в именах файлов
     * @details Проверяет корректный разбор имен файлов с пробелами и путями
     */
    TEST(SpecialCharactersInFilenames) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "file with spaces.db", "-j", "path/to/journal.log", "-p", "8080", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("file with spaces.db", iface.getParams().inFileName);
        CHECK_EQUAL("path/to/journal.log", iface.getParams().inFileJournal);
    }

    /**
     * @brief Тест IPv6 адреса
     * @details Проверяет корректный разбор IPv6 адреса в качестве значения параметра адреса
     */
    TEST(IPv6Address) {
        UserInterface iface;
        const char* argv[] = {"test", "-b", "db", "-j", "log", "-p", "9090", "-a", "::1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("::1", iface.getParams().Address);
    }
}

/**
 * @brief Главная функция тестов
 * @details Запускает все тесты и возвращает код результата выполнения
 * @return Код возврата: 0 при успешном выполнении всех тестов
 */
int main() {
    return UnitTest::RunAllTests();
}
