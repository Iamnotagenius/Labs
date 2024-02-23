# MP3-file metainformation editor
Implement a text metainformation editor for mp3 files.
ID3v2 standard should be used as metainformation format.
The editor should represent itself as a console application
accepting file name with `--filepath` flag as well as
one of the following commands:
1. `--show` - display all metainformation as a table
2. `--set=prop_name` `--value=prop_value` - set a value
of a specified metainformation field with the name *prop_name* to a value *prop_value*
3. `--get=prop_name` - output a metainformation field with the name *prop_name*

Example:
```
app.exe --filepath=Song.mp3 --show
app.exe --filepath=Song.mp3 --get=TIT2
app.exe --filepath=Song.mp3 --set=COMM --value=Test
```
### Note:
My program doesn't have `--show` flag, instead the table is displayed
when no additional flags have been passed.
For example: `app.exe "C418 - Aria Math.mp3"`

---
# Редактор метаинформации mp3-файла
Реализовать редактор текстовой метаинформации mp3 файла.
В качестве стандарта метаинформации принимаем ID3v2.
Редактор представлять из себя консольную программу
принимающую в качестве аргументов имя файла через параметра
`--filepath`, а также одну из выбранных команд:
1. `--show` - отображение всей метаинформации в виде таблицы
2. `--set=prop_name` `--value=prop_value` - выставляет значение
определенного поля метаинформации с именем *prop_name* в
значение *prop_value*
3. `--get=prop_name` - вывести определенное поле метаинформации с именем *prop_name*

Например:
```
app.exe --filepath=Song.mp3 --show
app.exe --filepath=Song.mp3 --get=TIT2
app.exe --filepath=Song.mp3 --set=COMM --value=Test
```
### Примечание:
Флага `--show` у моей программы нет, таблица метаинформации печатается,
если не указывать никаких доп. флагов. Например, `app.exe "C418 - Aria Math.mp3"`
