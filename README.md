## Транспортный каталог
---
Реализация системы автобусной маршрутизации, работающая с запросами в формате JSON. Программа двухстадийная,
и имеет 2 варианта запросов: первый вариант отвечает за создание, сериализацию и сохранение базы данных, 
второй загружает в себя сохранённую ранее базу и выдаёт ответы на запросы по любым оптимальным маршрутам, 
по остановкам и автобусам, отрисовке карты. Форматы вывода: JSON и SVG.



### Использование
---
* Установка всех требуемых компонентов для запуска приложения
* Сборка системой CMake
* Запуск с параметром командной строки `make_base` для создания базы
* Запуск с параметром командной строки `process_request` для получения информации по запросам
* Команды быстрой сборки и демонстрации работы, для систем Linux. (из каталога `build`):
```
    cmake ../
    cmake --build .
    ./transport_catalogue
``` 
   Подробности использования в файле `instructions.json`


### Требования
---
1. С++ 17 (STL)
2. GCC 10.2.1 или Clang 11.0.1
3. Protobuf 3.21.11 (В комплекте)
4. CMake 3.10



### Планы по доработке
---
* Добавить обработку невалидных запросов
* Сделать Графическое приложение для ПК

