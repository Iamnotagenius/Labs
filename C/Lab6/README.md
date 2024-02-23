# File archiver
The goal of this assignment would be developing an archiver program that will
allow saving several files into one archive.
Archiver should:
1. Be able to archive several (one or more) given files into archive with the `.arc` extension
2. Be able to unpack file archive, extracting initially archived files
3. List packed files in a given archive
4. Compress and decompress data while archiving using the Huffman algorithm
(this is optional task, do it for additional points)

Archiver should be a console application accepting the following arguments:
- `--file FILE` - Name of an archive the program would work with
- `--create` - Command for creating a new archive
- `--extract` - Command for extracting files from an archive
- `--list` - Command for listing files in an archive
- `FILE1 FILE2 ... FILEN` - Arguments for files to archive

Usage examples:
```
arc --file data.arc --create a.txt b.bin c.bmp
arc --file data.arc --extract
arc --file data.arc --list
```
---
# Архиватор файлов.
Целью лабораторной работы является разработка программы по архивированию
и распаковке нескольких файлов в один архив. Архиватор должен
1. Уметь архивировать несколько (один и более) указанных файлов в архив с
расширением *.arc
2. Уметь распаковывать файловых архив, извлекая изначально запаковонные
файлы
3. Предоставлять список файлов упакованных в архиве
4. Сжимать и разжимать данные при архивировании с помощью алгоритма
Хаффмана (опциональное задание, оценивается доп баллами)

Архиватор должен быть выполнен в виде консольного приложения,
принимающего в качестве аргументов следующий параметры:
- `--file FILE` - Имя файлового архива с которым будет работать архиватор
- `--create` - Команда для создания файлового архива
- `--extract` - Команда для извлечения из файлового архива файлов
- `--list` - Команда для предоставления списка файлов, хранящихся в архиве
- `FILE1 FILE2 ... FILEN` - Свободные аргументы для передачи списка файлов для запаковки

Примеры использования:
```
arc --file data.arc --create a.txt b.bin c.bmp
arc --file data.arc --extract
arc --file data.arc --list
```
