#include "gui_dashbord.h"
#include "vehicle_data.h"
#include "map_view.h"
#include <stdio.h>
static dashboard_layers_t dash;
#define TACHO_MAX_RPM   8000
#define SPEEDO_MAX_KMH  240
#define MAP_ZOOM        12
#define MAP_START_TILE_X 2344
#define MAP_START_TILE_Y 1481
static void create_gauge(lv_obj_t *container, lv_obj_t **arc_out, lv_obj_t **label_out,
                          int32_t min_val, int32_t max_val, const char *unit, int32_t size) {
    lv_obj_t *arc = lv_arc_create(container);
    lv_obj_set_size(arc, size, size);
    lv_obj_center(arc);
    lv_obj_set_style_bg_opa(arc, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(arc, lv_color_black(), 0);
    lv_obj_set_style_border_width(arc, 0, 0);
    lv_obj_set_style_pad_all(arc, 0, 0);
    lv_obj_set_style_radius(arc, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_arc_width(arc, 10, 0);
    lv_obj_set_style_arc_color(arc, lv_color_make(255, 0, 0), 0);
    lv_obj_set_style_arc_rounded(arc, false, 0);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_arc_set_rotation(arc, 0);
    lv_arc_set_range(arc, min_val, max_val);
    lv_arc_set_value(arc, min_val);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_clickable(arc, false);

    lv_obj_t *label = lv_label_create(container);
    if (LV_FONT_MONTSERRAT_36) {
        lv_obj_set_style_text_font(label, &lv_font_montserrat_36, 0);
    }
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_label_set_text_fmt(label, "0 %s", unit);
    lv_obj_center(label);

    *arc_out = arc;
    *label_out = label;
}

void dashboard_init(void) {
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    dash.layer_bg_map = lv_obj_create(scr);
    lv_obj_remove_style_all(dash.layer_bg_map);
    lv_obj_set_size(dash.layer_bg_map, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollable(dash.layer_bg_map, false);
    lv_obj_set_style_bg_opa(dash.layer_bg_map, LV_OPA_TRANSP, 0);
    dash.map_obj = map_view_create(dash.layer_bg_map, "C:/Users/ionut/Desktop/mapsIonut");
    lv_obj_set_size(dash.map_obj, LV_PCT(100), LV_PCT(100));
    lv_obj_align(dash.map_obj, LV_ALIGN_CENTER, 0, 0);
    map_view_set_tile(dash.map_obj, MAP_ZOOM, MAP_START_TILE_X, MAP_START_TILE_Y);
    dash.layer_gauges = lv_obj_create(scr);
    lv_obj_remove_style_all(dash.layer_gauges);
    lv_obj_set_size(dash.layer_gauges, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollable(dash.layer_gauges, false);
    dash.tacho_container = lv_obj_create(dash.layer_gauges);
    lv_obj_remove_style_all(dash.tacho_container);
    lv_obj_set_size(dash.tacho_container, 260, 260);
    lv_obj_align(dash.tacho_container, LV_ALIGN_BOTTOM_LEFT, 35, 10);
    lv_obj_set_style_bg_opa(dash.tacho_container, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(dash.tacho_container, lv_color_black(), 0);
    lv_obj_set_style_radius(dash.tacho_container, LV_RADIUS_CIRCLE, 0);
    create_gauge(dash.tacho_container, &dash.tacho_arc, &dash.tacho_label,
                 0, TACHO_MAX_RPM, "RPM", 220);
    dash.speedo_container = lv_obj_create(dash.layer_gauges);
    lv_obj_remove_style_all(dash.speedo_container);
    lv_obj_set_size(dash.speedo_container, 260, 260);
    lv_obj_align(dash.speedo_container, LV_ALIGN_BOTTOM_RIGHT, -35, 10);
    lv_obj_set_style_bg_opa(dash.speedo_container, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(dash.speedo_container, lv_color_black(), 0);
    lv_obj_set_style_radius(dash.speedo_container, LV_RADIUS_CIRCLE, 0);
    create_gauge(dash.speedo_container, &dash.speedo_arc, &dash.speedo_label,
                 0, SPEEDO_MAX_KMH, "km/h", 220);
    dash.layer_top_bar = lv_obj_create(scr);
    lv_obj_remove_style_all(dash.layer_top_bar);
    lv_obj_set_size(dash.layer_top_bar, LV_PCT(100), 40);
    lv_obj_align(dash.layer_top_bar, LV_ALIGN_TOP_MID, 0, 10);

    dash.lbl_range = lv_label_create(dash.layer_top_bar);
    lv_label_set_text(dash.lbl_range, "-- km");
    lv_obj_set_style_text_color(dash.lbl_range, lv_color_white(), 0);
    lv_obj_align(dash.lbl_range, LV_ALIGN_LEFT_MID, 120, 0);
    dash.layer_bottom_bar = lv_obj_create(scr);
    lv_obj_remove_style_all(dash.layer_bottom_bar);
    lv_obj_set_size(dash.layer_bottom_bar, LV_PCT(100), 40);
    lv_obj_align(dash.layer_bottom_bar, LV_ALIGN_BOTTOM_MID, 0, -10);

    dash.lbl_info = lv_label_create(dash.layer_bottom_bar);
    lv_label_set_text(dash.lbl_info, "--:--   --.-C");
    lv_obj_set_style_text_color(dash.lbl_info, lv_color_white(), 0);
    lv_obj_align(dash.lbl_info, LV_ALIGN_CENTER, 0, 0);

}

void dashboard_update_from_vehicle_data(void) {
    vehicle_data_t data;
    vehicle_data_get(&data); // daca Pico e deconectat, toate campurile sunt 0

    // 1. Actualizare ceasuri
    lv_arc_set_value(dash.tacho_arc, (int32_t)data.rpm);
    lv_label_set_text_fmt(dash.tacho_label, "%d RPM", (int)data.rpm);

    lv_arc_set_value(dash.speedo_arc, (int32_t)data.speed_kmh);
    lv_label_set_text_fmt(dash.speedo_label, "%d km/h", (int)data.speed_kmh);

    // 2. Actualizare dinamică hartă direct din datele GPS primite de la Pico
    if (dash.map_obj && data.connected &&
        data.latitude != 0.0f && data.longitude != 0.0f) {
        map_view_update_gps(dash.map_obj, MAP_ZOOM, (double)data.latitude, (double)data.longitude);
    }

    // Culoare dinamica in functie de conexiune
    lv_color_t color = data.connected ? lv_color_white() : lv_color_make(120, 120, 120);
    lv_obj_set_style_text_color(dash.tacho_label, color, 0);
    lv_obj_set_style_text_color(dash.speedo_label, color, 0);
}