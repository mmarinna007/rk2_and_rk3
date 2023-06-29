<div align="center">

# JSON && CSV format

RK2: Конвертер:
    * JSON -> CSV
    * CSV -> JSON

RK3: Парсер. Импорт в хеш-таблицу и последующий экспорт в другой формат
    * JSON -> CSV
    * CSV -> JSON

</div>


### Компиляция RK2
```
make rk2
```

### Компиляция RK3
```
make rk3
```

### Запуск
Скомпилированная программа находится в директории **build**
Пример запуска
```
./build/rk2 data.json
./build/rk3 city.json
```
