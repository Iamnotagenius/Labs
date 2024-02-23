# Server log analysis
You are given an access.log from a NASA server ([download](https://drive.google.com/file/d/1jjzMocc0Rn9TqkK_51Oo93Fy78KYnm2i/view)).
This is a text file, each line of which has the following format:
```
$remote_addr - - [$local_time] “$request” $status $bytes_send
```
1. `$remote_addr` - a source of a request
2. `$local_time` - time of a request
3. `$request` - a request itself
4. `$status` - a status code of a request
5. `$bytes_send` - amount of bytes recieved in a response

Example:
```
198.112.92.15 - - [03/Jul/1995:10:50:02 -0400] "GET /shuttle/countdown/
HTTP/1.0" 200 3985
```
Required:
1. Collect a list of failed requests with 5xx codes with the amount of such requests
2. Find a time window (with parameterized length) with the highest amount of requests
---
# Анализ логов сервера
Вам предоставлен access.log одного из серверов NASA ([скачать](https://drive.google.com/file/d/1jjzMocc0Rn9TqkK_51Oo93Fy78KYnm2i/view)).
Это текстовый файл, каждая строка которого имеет следующий формат:
```
$remote_addr - - [$local_time] “$request” $status $bytes_send
```
1. `$remote_addr` - источник запроса
2. `$local_time` - время запроса
3. `$request` - запрос
4. `$status` - статус ответ
5. `$bytes_send` - количество переданных в ответе байт

Например:
```
198.112.92.15 - - [03/Jul/1995:10:50:02 -0400] "GET /shuttle/countdown/
HTTP/1.0" 200 3985
```
Требуется:
1. Подготовить список запросов, которые закончились 5xx ошибкой, с
количеством неудачных запросов
2. Найти временное окно (длительностью параметризуются), когда
количество запросов на сервер было максимально