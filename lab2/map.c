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
* СДЕЛАТЬ:
* Просуммируйте все символы в строке и поделите результат по модулю 
* MAP_SIZE
*/
}

/* Добавляет элемент к хэш таблице */
void map_add( COUNTRY ** map, char * name, int population, int area)
{
/* 
* СДЕЛАТЬ:
*/    
}

/* Удаляет страну с указанным названием из хеш таблицы */
void map_delete(COUNTRY ** map, char * name)
{
/* 
* СДЕЛАТЬ:
*/ 
}

/* Ищет страну с указанным названием в хеш таблице */
COUNTRY * map_find(COUNTRY ** map, char * name)
{
/* 
* СДЕЛАТЬ:
*/ 
}

/* Печатает на экране все записи хеш таблицы */
void map_dump(COUNTRY ** map)
{
/* 
* СДЕЛАТЬ:
*/
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
        clear(map[cnt]);
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


