#include "map_view.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif

#define TILE_PX   256   // dimensiunea standard a unui tile OSM (256x256)
#define GRID_SIZE 5     // grila 5x5 pentru a acoperi ecranul desktop fara bare negre laterale

typedef struct {
    lv_obj_t *container;
    lv_obj_t *tiles[GRID_SIZE][GRID_SIZE];
    char      folder_path[128];
    char      img_paths[GRID_SIZE][GRID_SIZE][160]; // buffer stabil pt lv_img_set_src
} map_view_t;

// O singura harta activa in acest MVP (poti extinde la un array daca vrei mai multe)
static map_view_t g_map;
static int g_last_zoom = -1;
static int g_last_tile_x = -1;
static int g_last_tile_y = -1;
static int g_map_zoom = 12;

lv_obj_t *map_view_create(lv_obj_t *parent, const char *folder_path) {
    memset(&g_map, 0, sizeof(g_map));
    g_last_zoom = -1;
    g_last_tile_x = -1;
    g_last_tile_y = -1;
    g_map_zoom = 12;
    strncpy(g_map.folder_path, folder_path, sizeof(g_map.folder_path) - 1);

    g_map.container = lv_obj_create(parent);
    lv_obj_remove_style_all(g_map.container);
    lv_obj_set_size(g_map.container, GRID_SIZE * TILE_PX, GRID_SIZE * TILE_PX);
    lv_obj_center(g_map.container);
    lv_obj_set_scrollable(g_map.container, false);

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            lv_obj_t *img = lv_img_create(g_map.container);
            lv_obj_set_pos(img, col * TILE_PX, row * TILE_PX);
            lv_obj_set_size(img, TILE_PX, TILE_PX);
            g_map.tiles[row][col] = img;
        }
    }

    return g_map.container;
}
void map_view_set_tile(lv_obj_t *map_obj, int zoom, int tile_x, int tile_y) {
    (void)map_obj; // in acest MVP folosim direct g_map; poti extinde cu lookup daca ai mai multe harti

    g_map_zoom = zoom;
    g_last_zoom = zoom;
    g_last_tile_x = tile_x;
    g_last_tile_y = tile_y;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            int dx = col - 1; // -1, 0, +1
            int dy = row - 1;

            char base_path[256];
            const char *folder = g_map.folder_path[0] != '\0' ? g_map.folder_path : ".";
            const bool has_drive_or_absolute = folder[0] == '/' || folder[0] == '\\' ||
                                               (folder[1] == ':' && ((folder[0] >= 'A' && folder[0] <= 'Z') ||
                                                                    (folder[0] >= 'a' && folder[0] <= 'z')));

            if (has_drive_or_absolute) {
                snprintf(base_path, sizeof(base_path), "%s/%d/%d/%d.png", folder, zoom, tile_x + dx, tile_y + dy);
            } else {
                snprintf(base_path, sizeof(base_path), "%s/%d/%d/%d.png", folder, zoom, tile_x + dx, tile_y + dy);
            }

            snprintf(g_map.img_paths[row][col], sizeof(g_map.img_paths[row][col]), "%s", base_path);
                 lv_image_header_t header;
                 lv_result_t decode_res = lv_image_decoder_get_info(g_map.img_paths[row][col], &header);
                 printf("[map_view] %s: decode=%d %ux%u\n",
                     g_map.img_paths[row][col], (int)decode_res,
                     (unsigned)header.w, (unsigned)header.h);
            lv_img_set_src(g_map.tiles[row][col], g_map.img_paths[row][col]);
        }
    }
}
void map_view_set_zoom(lv_obj_t *map_obj, int zoom) {
    if (zoom < 8) zoom = 8;
    if (zoom > 18) zoom = 18;

    if (g_last_tile_x >= 0 && g_last_tile_y >= 0) {
        map_view_set_tile(map_obj, zoom, g_last_tile_x, g_last_tile_y);
    } else {
        g_map_zoom = zoom;
        g_last_zoom = zoom;
    }
}

int map_view_get_zoom(lv_obj_t *map_obj) {
    (void)map_obj;
    return g_map_zoom;
}

void map_view_update_gps(lv_obj_t *map_obj, int zoom, double lat, double lon) {
    int target_zoom = zoom > 0 ? zoom : g_map_zoom;

    // Formulă matematică de conversie Lat/Lon -> Tile X/Y (OpenStreetMap Mercator)
    double lat_rad = lat * M_PI / 180.0;
    int n = 1 << target_zoom; // 2^zoom
    
    int tile_x = (int)floor((lon + 180.0) / 360.0 * n);
    int tile_y = (int)floor((1.0 - log(tan(lat_rad) + (1.0 / cos(lat_rad))) / M_PI) / 2.0 * n);

    if (target_zoom == g_last_zoom && tile_x == g_last_tile_x && tile_y == g_last_tile_y) {
        return;
    }

    g_last_zoom = target_zoom;
    g_last_tile_x = tile_x;
    g_last_tile_y = tile_y;

    // Actualizează rețeaua 3x3 de tile-uri
    map_view_set_tile(map_obj, target_zoom, tile_x, tile_y);
}

void map_view_set_visible(lv_obj_t *map_obj, bool visible) {
    lv_obj_set_hidden(map_obj, !visible);
}