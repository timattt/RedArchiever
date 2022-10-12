# Красный арихиватор

## Консольная версия

### Шифрование файлов

```
redarch -e file1 file2 ... fileN KEY outFile
```

Где:

* file_i - путь до файла, который надо упаковать
* KEY - ключ, который будет использоваться для шифрования
* outFile - название файла, куда запишем результат шифрования


### Расшифровка файлов

```
redarch -d inFile KEY outDir 
```

Где:

* inFile - файл, который надо расшифровать
* KEY - ключ, который будет использоваться для расшифровки
* outDir - название директории, куда положим итоговые файлы

## Структура архива

* 4 байта - кол файлов - N
* N с строк. И нулем на конце - указывают на имена
* N указателей в файле по 4 байта - уазывают на начало данных файлов
* данные из файлов