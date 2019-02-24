#ifndef _LIST_H_
#define _LIST_H_

typedef struct _COUNTRY {
    char name[256];
    int population;
    int area;
    struct _COUNTRY * next;
} COUNTRY;


int add( COUNTRY ** list, char * name, int population, int area);
void delete(COUNTRY ** list, COUNTRY * v);
COUNTRY * find(COUNTRY * list, char * name);
int count(COUNTRY * list);
int sort_by_name(COUNTRY ** list);
int sort_by_area(COUNTRY ** list);
int sort_by_population(COUNTRY ** list);
void dump(COUNTRY * list);
void print_country(COUNTRY * p);
void clear(COUNTRY * list);
COUNTRY * load();
void save(COUNTRY * list);

#endif // _LIST_H_

