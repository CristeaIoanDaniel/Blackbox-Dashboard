#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include "lvgl/lvgl.h"

// Creeaza componenta de harta in interiorul parintelui dat.
// folder_path = calea catre folderul de pe SD card care contine tile-urile,
// ex: "/media/sdcard/tiles/bucuresti" (Linux) sau litera de drive pe Windows.
//
// Se asteapta structura: folder_path/{zoom}/{x}/{y}.png  (stil OSM standard)
// Daca tile-urile tale NU respecta acest format (ex: nume gen tile_1.png),
// spune-mi exact cum sunt denumite si adaptez map_view_set_tile().
lv_obj_t *map_view_create(lv_obj_t *parent, const char *folder_path);

// Schimba tile-ul central afisat (zoom / x / y conform schemei slippy map).
// Afiseaza o grila 3x3 in jurul tile-ului central, ca sa umple ecranul
// chiar daca masina/harta se misca putin.
void map_view_set_tile(lv_obj_t *map_obj, int zoom, int tile_x, int tile_y);

// Ascunde / arata harta (folosit de butonul de View toggle).
void map_view_set_visible(lv_obj_t *map_obj, bool visible);
void map_view_set_zoom(lv_obj_t *map_obj, int zoom);
int map_view_get_zoom(lv_obj_t *map_obj);
void map_view_update_gps(lv_obj_t *map_obj, int zoom, double lat, double lon);
#endif // MAP_VIEW_H