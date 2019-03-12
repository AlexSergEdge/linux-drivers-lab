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

#include <ctype.h>
#include <wchar.h>
#include <wctype.h>


// Функция замены нижнего подчеркивания в названии страны на пробел
void undecorate_name(char * name) {
    int cnt = 0;
    while (name[cnt]) {
        if (name[cnt] == '_')
            name[cnt] = ' ';
        cnt++;
    }
}

int main(int argc, char * argv[])
{
    setlocale(LC_ALL, "ru_RU.UTF8");
    COUNTRY ** map = (COUNTRY **)map_load();
    // Переменные для населения и площади
    int population;
    int area;
    // Выделяем массивы под имя и команду
    char name_static[255];
    char cmd_static[16];

    for(;;) {
        printf("Введите команду: \n");
        scanf("%s", &cmd_static[0]);
        if (strcmp(cmd_static, "add") == 0) {
            scanf("%s", &name_static[0]);
            scanf("%d", &population);
            scanf("%d", &area);
            undecorate_name(&name_static[0]);
            printf("Добавляю страну: %s\n", name_static);
            map_add(map, &name_static[0], population, area);
        }
        else if (strcmp(cmd_static, "delete") == 0) {
            scanf("%s", &name_static[0]);
            undecorate_name(&name_static[0]);
            printf("Удаляю страну: %s\n", name_static);
            COUNTRY * del_map = (COUNTRY *)map_find(map, &name_static[0]);
            map_delete(map, del_map);
        }
        else if (strcmp(cmd_static, "dump") == 0) {
            printf("Вывожу страны\n");
            map_dump(map);
        }
        else if (strcmp(cmd_static, "view") == 0) {
            scanf("%s", &name_static[0]);
            undecorate_name(&name_static[0]);
            printf("Вывожу информацию о стране: %s\n", name_static);
            printf("|%-20s|%-20s|%-20s|\n", "Name", "Popularity", "Area");
            printf("|--------------------|--------------------|--------------------|\n");
            COUNTRY * info_map = (COUNTRY *)map_find(map, &name_static[0]);
            print_country(info_map);
        }
        else if (strcmp(cmd_static, "count") == 0) {
            printf("Вывожу количество стран: ");
            printf("%i\n",map_count(map));
        }
        else if (strcmp(cmd_static, "save") == 0) {
            printf("Сохраняю в базу данных\n");
            map_save(map);
        }
        else if (strcmp(cmd_static, "quit") == 0) {
            printf("Завершаю работу\n");
            break;
        }
        else {
            printf("Неизвестная команда\n");
        }
    }
    
    map_clear(map);
    
    return 0;
}

