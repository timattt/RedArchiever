# Красный арихиватор

## Консольная версия

### Сборка

1. Склонируйте репозиторий, откройте консоль в его корневой папке.
2. Введите команды:   

**Сборка под unix:**
```
mkdir build
cd build
cmake ..
make
```
**Сборка под MinGW**
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make.exe
```
3. Теперь в папке build будет лежать два бинарных файла - программа и тесты к ней.

### Шифрование файлов

```
redarch -e file1 file2 ... fileN KEY outFile
```

Где:

* file_i - путь до файла, который надо упаковать
* KEY - ключ, который будет использоваться для шифрования
* outFile - название файла, куда запишем результат шифрования

**Внимание! Надо указаывать полные пути до файлов везде!**

### Расшифровка файлов

```
redarch -d inFile KEY outDir 
```

Где:

* inFile - файл, который надо расшифровать
* KEY - ключ, который будет использоваться для расшифровки
* outDir - название директории, куда положим итоговые файлы

**Внимание! Надо указаывать полные пути до файлов везде!**

### Возвращаемые значения

Если программа вернула не ноль, значит что-то сломалось.
И будет возвращен соответствующий код об ошибке.

Коды:

* ARGS_ERROR 1 - плохие аргументы
* FILES_ERROR 2 - плохие файлы
* MEMORY_ERROR 3 - плохо с памятью

## Версия с графическим интерфейсом

### Сборка

1. Склонируйте репозиторий.
2. Зайдите в папку Sources/UI
3. Там есть файл RedArchiver.pro
4. Откройте его с помощью QT Creator.
5. Там же можно собрать, запустить и тд

### Устройство

![image](https://user-images.githubusercontent.com/25401699/203058266-e19e9fe9-9317-4b48-8f66-e87bc621c511.png)

Чтобы зашифровать, нажмите **browse files**. Выберете один или несколько файлов для шифровки. Затем нажмите **encrypt** и выберете файл,
чтобы записать в него результат.      
Чтобы расшифровать файл, нажмите **browse files**. Выберете архив. Потом нажмите **decrypt**. Теперь выберете дикерторию, куда разархивировать файлы.
Не забудьте указывать верные ключи для корректной расшифровки ваших архивов.
