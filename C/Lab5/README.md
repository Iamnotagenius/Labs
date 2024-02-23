# Game of Life
The goal of this assignment will be implementation of a "Game of Life",
allowing to output game generation into a BMP-formatted monochrome picture.
The assignment should be done as a console application
accepting the following arguments:
1. `--input input_file.bmp` -
Where input_file.bmp - monochrome picture in BMP format,
storing the initial state (first generation) of a game
2. `--output dir_name` -
Name of a directory where generations would be stored
3. `--max_iter N` -
Maximum number of generations which would be emulated. Optional parameter. Infinity is the default.
4. `--dump_freq N` -
The frequency at which generations would be stored as a picture. The default is each generation. 
---
# Игра жизнь
Целью лабораторной работы является реализация игры “Жизнь” , позволяющая
выводить поколение игры в монохромную картинку в формате BMP. Плоскость
“вселенной” игры ограничена положительными координатами.
Лабораторная работы должна быть выполнена в виде консольного приложения
принимающего в качестве аргументов следующие параметры:
1. `--input input_file.bmp` -
Где input_file.bmp - монохромная картинка в формате bmp,
хранящая начальную ситуация (первое поколение) игры
2. `--output dir_name` -
Название директории для хранения поколений игры в виде
монохромной картинки
3. `--max_iter N` -
Максимальное число поколений которое может эмулировать
программа. Необязательный параметр, по-умолчанию бесконечность
4. `--dump_freq N` -
Частота с которой программа должно сохранять поколения виде
картинки. Необязательный параметр, по-умолчанию равен 1
