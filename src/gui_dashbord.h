#ifndef GUI_DASHBOARD_H
#define GUI_DASHBOARD_H

#include "lvgl/lvgl.h"
typedef struct {
    lv_obj_t *layer_bg_map;     
    lv_obj_t *map_obj;          
    lv_obj_t *layer_gauges;     
    lv_obj_t *tacho_container;  
    lv_obj_t *speedo_container; 
    lv_obj_t *tacho_arc;
    lv_obj_t *speedo_arc;
    lv_obj_t *tacho_label;
    lv_obj_t *speedo_label;
    lv_obj_t *layer_top_bar;    
    lv_obj_t *lbl_range;
    lv_obj_t *layer_bottom_bar;
    lv_obj_t *lbl_info;
} dashboard_layers_t;

void dashboard_init(void);
void dashboard_update_from_vehicle_data(void);

#endif 