#ifndef _MAP_H_
#define _MAP_H_


COUNTRY ** map_create(void);
void map_clear(COUNTRY ** map);
void map_add( COUNTRY ** map, char * name, int population, int area);
void map_delete(COUNTRY ** map, char * name);
COUNTRY * map_find(COUNTRY ** map, char * name);
void map_dump(COUNTRY ** map);
COUNTRY ** map_load();
void map_save(COUNTRY ** map);


#endif // _MAP_H_
