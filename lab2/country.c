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

// Функция замены нижнего подчеркивания в названии страны на пробел
void undecorate_name(char * name) {
    int cnt;
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
    // Выделяем динамически память под команду и название страны
    char * cmd = (char *)malloc(sizeof(char));
    char name_static[255];

    for(;;) {
        printf("Введите команду: \n");
        scanf("%s", cmd);
        if (strcmp(cmd, "add") == 0) {
            //char * name = (char *)malloc(sizeof(char));
            scanf("%s", &name_static[0]);
            scanf("%d", &population);
            scanf("%d", &area);
            undecorate_name(&name_static[0]);
            printf("Добавляю страну: %s\n", name_static);
            map_add(map, &name_static[0], population, area);
            //free(name);
        }
        else if (strcmp(cmd, "delete") == 0) {
            //char * name = (char *)malloc(sizeof(char));
            scanf("%s", &name_static[0]);
            undecorate_name(&name_static[0]);
            printf("Удаляю страну: %s\n", name_static);
            COUNTRY * del_map = (COUNTRY *)map_find(map, &name_static[0]);
            map_delete(map, del_map);
            //free(name);
        }
        else if (strcmp(cmd, "dump") == 0) {
            printf("Вывожу страны\n");
            map_dump(map);
        }
        else if (strcmp(cmd, "view") == 0) {
            //char * name = (char *)malloc(sizeof(char));
            scanf("%s", &name_static[0]);
            undecorate_name(&name_static[0]);
            printf("Вывожу информацию о стране: %s\n", name_static);
            printf("|%-20s|%-20s|%-20s|\n", "Name", "Popularity", "Area");
            printf("|--------------------|--------------------|--------------------|\n");
            COUNTRY * info_map = (COUNTRY *)map_find(map, &name_static[0]);
            print_country(info_map);
            //free(name);
        }
        else if (strcmp(cmd, "count") == 0) {
            printf("Вывожу количество стран: ");
            printf("%i\n",map_count(map));
        }
        else if (strcmp(cmd, "save") == 0) {
            printf("Сохраняю в базу данных\n");
            map_save(map);
        }
        else if (strcmp(cmd, "quit") == 0) {
            printf("Завершаю работу\n");
            break;
        }
        else {
            printf("Неизвестная команда\n");
        }
    }

    // Очищаем динамически выделенную память
    free(cmd);
    
    map_clear(map);
    
    return 0;
}

