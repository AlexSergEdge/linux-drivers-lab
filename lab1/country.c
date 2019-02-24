/*
* Задание #6
* Автор: Бринк А.С.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int main(int argc, char * argv[])
{
    if (argc == 1) {
        printf("Необходимо указать аргументы!\n");
        return 0;
    }

    //COUNTRY * list;

    /* Загрузка списка */
    //list = load();

    /* DONE:  
     * Обработка команд из командной строки
    */
    if (strcmp(argv[1], "add") == 0) {
        // Если число агрументов не равно 4 - ошибка
        if (argc != 5) {
            printf("Usage: country add <name> <population> <area>\n");
            //clear(&list);
            return 0;
        }
        printf("Добавляю страну...\n");      
                                    
    }
    else if (strcmp(argv[1], "delete") == 0) {
        // Если число агрументов не равно 3 - ошибка
        if (argc != 3) {
            printf("Usage: country delete <name>\n");
            //clear(&list);
            return 0;
        } 
        printf("Удаляю страну...\n");                                    
    }
    else if (strcmp(argv[1], "dump") == 0) {
        // Если число агрументов больше 3 - ошибка
        if (argc > 3) {
            printf("Usage: country dump [<flag>]\n");
            //clear(&list);
            return 0;
        }
        printf("Вывожу страны...\n");        
          
    }
    else if (strcmp(argv[1], "view") == 0) {
        // Если число аргументов не равно 3 - ошибка
        if (argc != 3) {
            printf("Usage: country view <name>\n");
            //clear(&list);
            return 0;
        }
        printf("Вывожу информацию о стране...\n");       
    
    }
    else if (strcmp(argv[1], "count") == 0) {
        // Если число аргументов больше 2 - ошибка
        if (argc > 2) {
            printf("Usage: country count\n");
            //clear(&list);
            return 0;
        }
        printf("Вывожу число стран...\n"); 
    }
    else {
        printf("Неизвестная команда!\n");
        //clear(&list);
        return 0;
    }

    /* Удаление списка из динамической памяти */
    //clear(&list);

    return 0;
}

