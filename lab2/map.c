#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define DBNAME "country_list"
#define MAP_SIZE 64
#define MAX_LINE_LENGTH 255


/* 
* Возвращает значение хэша ключа. Ключ это название страны.
*/
unsigned int hash(char * key)
{
/* 
* Просуммируем все символы в строке и поделим результат по модулю 
* MAP_SIZE
*/
    unsigned int i = 0;
    unsigned int sum = 0;
    while (key[i]) {
        sum += key[i];
        i++;
    }
    return i % MAP_SIZE;
}

/* Добавляет элемент к хэш таблице */
void map_add( COUNTRY ** map, char * name, int population, int area)
{
    // Получаем значение хэша ключа name
    //COUNTRY * country = map_find(map, name);
    int hash_value = hash(name);
    COUNTRY * country = find(map[hash_value], name);    
    if (country) {
        printf("Страна уже есть в хэш-таблице, обновляю данные\n");
        country->population = population;
        country->area = area;
    }
    else {
        int res = add(&map[hash_value], name, population, area);     
    }
}

/* Удаляет страну с указанным названием из хеш таблицы */
void map_delete(COUNTRY ** map, char * name)
{
    int hash_value = hash(name);
    COUNTRY * country = find(map[hash_value], name);
    if (country) {    
        print_country(country);
        delete(&map[hash_value], country);
    }
    else {
        printf("Страны %s не существует!\n", name);
    }
}

/* Ищет страну с указанным названием в хеш таблице */
COUNTRY * map_find(COUNTRY ** map, char * name)
{
    int hash_value = hash(name);
    return find(map[hash_value], name);
}

/* Печатает на экране все записи хеш таблицы */
void map_dump(COUNTRY ** map)
{
    printf("|%-20s|%-20s|%-20s|\n", "Name", "Popularity", "Area");
    printf("|--------------------|--------------------|--------------------|\n");
    COUNTRY * pointer;
    int i = 0;
    while (i != MAP_SIZE) {
        if (map[i]) {
            dump(map[i]);
        }        
        i++;
    }
}

/* Подсчет количества элементов */
int map_count(COUNTRY ** map)
{
    COUNTRY * pointer;
    int i = 0;
    int summ = 0;
    while (i != MAP_SIZE) {
        if (map[i]) {
            summ += count(map[i]);
        }
        i++;
    }
    return summ;
}

/* Создает хэш таблицу */
COUNTRY ** map_create(void)
{
    COUNTRY ** map = (COUNTRY **)malloc(sizeof(COUNTRY *) * MAP_SIZE);
    memset(map, 0, sizeof(COUNTRY *) * MAP_SIZE);
    return map;
}

/* Удаляет хэш таблицу */
void map_clear(COUNTRY ** map)
{
    int cnt;
    for (cnt = 0; cnt < MAP_SIZE; cnt++)
        // Для согласованности типов добавил &        
        clear(&map[cnt]);
    free(map);
}


/* Загружает таблицу из файла */
COUNTRY ** map_load()
{
    char buf[MAX_LINE_LENGTH + 1];
    char * par[3];
    int cnt, pcount = 0;
    COUNTRY *p, ** map = NULL;
    FILE * f = fopen(DBNAME, "r");

    map = map_create();

    buf[MAX_LINE_LENGTH] = 0x00; 
 
    if (f) {
        while(fgets(buf, MAX_LINE_LENGTH, f)) {
            pcount = 0;
            par[pcount++] = buf;
            cnt = 0;
            while(buf[cnt]) {
                if (buf[cnt] == ',') {
                    buf[cnt] = 0x00;
                    par[pcount++] = &buf[cnt + 1];
                }
                cnt++;
            }
            if (pcount == 3) {
                map_add(map, par[0], atoi(par[1]), atoi(par[2])); 
            }           
        }
        fclose(f);
    }
    return map;
}

/* Сохраняет таблицу в файл */
void map_save(COUNTRY ** map)
{
    COUNTRY * p;
    int cnt;
    FILE * f = fopen(DBNAME, "w+");

    if (f) {
        for (cnt = 0; cnt < MAP_SIZE; cnt++) {
            p = map[cnt];
            while (p != NULL) {
                fprintf(f, "%s,%d,%d\n", p->name, p->population, p->area);
                p = p->next;
            }
        }
        fclose(f);
    }
}


