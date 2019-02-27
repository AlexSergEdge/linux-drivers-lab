/*
* Задание #6
* Автор: Бринк А.С.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <locale.h>

int main(int argc, char * argv[])
{
    setlocale(LC_ALL, "ru_RU.UTF8");
    if (argc == 1) {
        printf("Необходимо указать аргументы!\n");
        return 0;
    }

    COUNTRY * list;
    
    int res;
    int population;
    int area;

    /* Загрузка списка */
    list = load();

    /* DONE:  
     * Обработка команд из командной строки
    */
    if (strcmp(argv[1], "add") == 0) {
        // Если число агрументов не равно 4 - ошибка
        if (argc != 5) {
            printf("Usage: country add <name> <population> <area> (if <population> or <area> are unknown, then type 0)\n");
            clear(&list);
            return 0;
        }
        printf("Добавляю страну...\n");
        population = atoi(argv[3]);
        area = atoi(argv[4]);
        res = add(&list, argv[2], population, area);
        if (res == 1) {
            clear(&list);
            return 0;
        }                                 
    }
    else if (strcmp(argv[1], "delete") == 0) {
        // Если число агрументов не равно 3 - ошибка
        if (argc != 3) {
            printf("Usage: country delete <name>\n");
            clear(&list);
            return 0;
        } 
        printf("Удаляю страну...\n");
        // Ищем страну по имени
        COUNTRY * del_country;
        del_country = find(list, argv[2]);
        if (!del_country) {
            printf("Такой страны нет в базе данных\n");
            clear(&list);
            return 0;
        }
        // Выводим информацию об удаляемой стране
        printf("|%-20s|%-20s|%-20s|\n", "Name", "Popularity", "Area");
        printf("|--------------------|--------------------|--------------------|\n");
        print_country(del_country);
        // Удаляем страну
        delete(&list, del_country);                                            
    }
    else if (strcmp(argv[1], "dump") == 0) {
        // Если число агрументов больше 3 - ошибка
        if (argc > 3) {
            printf("Usage: country dump [<flag>]\n");
            clear(&list);
            return 0;
        }
        if (argc == 3) {
            if (strcmp(argv[2], "-n") == 0) {
                printf("Сортирую по имени...\n");
                sort_by_name(&list);
            }
            else if (strcmp(argv[2], "-a") == 0) {
                printf("Сортирую по площади...\n");
                sort_by_area(&list);
            }

            else if (strcmp(argv[2], "-p") == 0) {
                printf("Сортирую по популяции...\n");
                sort_by_population(&list);
            }           
        }
        printf("Вывожу страны...\n");
        dump(list);          
    }
    else if (strcmp(argv[1], "view") == 0) {
        // Если число аргументов не равно 3 - ошибка
        if (argc != 3) {
            printf("Usage: country view <name>\n");
            clear(&list);
            return 0;
        }
        printf("Вывожу информацию о стране...\n");
        printf("|%-20s|%-20s|%-20s|\n", "Name", "Popularity", "Area");
        printf("|--------------------|--------------------|--------------------|\n");
        COUNTRY * info_country;        
        info_country = find(list, argv[2]);
        if (info_country == NULL) {
            printf("Такой страны нет в базе данных\n");
            clear(&list);
            return 0;
        }
        // Выводим информацию о стране
        print_country(info_country);
               
    }
    else if (strcmp(argv[1], "count") == 0) {
        // Если число аргументов больше 2 - ошибка
        if (argc > 2) {
            printf("Usage: country count\n");
            clear(&list);
            return 0;
        }
        printf("Вывожу число стран...\n");
        res = count(list);
        printf("Количество стран в базе данных: %i\n", res); 
    }
    else {
        printf("Неизвестная команда!\n");
        clear(&list);
        return 0;
    }
    // Сохраняем в файл
    save(list);
    /* Удаление списка из динамической памяти */
    clear(&list);

    return 0;
}

