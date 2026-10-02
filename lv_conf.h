/**
 * @file lv_conf.h
 * Configuration file for LVGL v9
 */

/* clang-format off */
#if 1 /* Enable content */
#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*====================
   COLOR SETTINGS
 *====================*/
/** Color depth: 16 (RGB565) or 32 (ARGB8888) in functie de afisaj */
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** Dimensiunea memoriei RAM alocate pentru LVGL (ex: 64 KB) */
    #define LV_MEM_SIZE (8 * 1024 * 1024)
    #define LV_MEM_POOL_EXPAND_SIZE 0
    #define LV_MEM_ADR 0
#endif

/*====================
   HAL SETTINGS
 *====================*/
#define LV_DEF_REFR_PERIOD  33   /**< Perioda de refresh (ms) ~ 30 FPS */
#define LV_DPI_DEF          130

/*=================
 * OPERATING SYSTEM
 *=================*/
#define LV_USE_OS   LV_OS_NONE   /**< Trece pe LV_OS_FREERTOS daca folosesti RTOS */

/*========================
 * RENDERING CONFIGURATION
 *========================*/
#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_ALIGN        4
#define LV_USE_DRAW_SW           1

#if LV_USE_DRAW_SW
    #define LV_DRAW_SW_SUPPORT_RGB565   1
    #define LV_DRAW_SW_SUPPORT_RGB888   1
    #define LV_DRAW_SW_SUPPORT_ARGB8888 1
    #define LV_DRAW_SW_DRAW_UNIT_CNT    1
    #define LV_DRAW_SW_COMPLEX          1
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
#define LV_USE_LOG 0  /**< Seteaza pe 1 pentru debugging pe UART/Serial */
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1

/*==================
 *   FONT USAGE
 *===================*/
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_36 1  /**< Pentru afisat viteza / turatia */
#define LV_FONT_MONTSERRAT_48 1  /**< Font mare pentru valori principale */

#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*==================
 * WIDGETS (DASHBOARD)
 *==================*/
#define LV_WIDGETS_HAS_DEFAULT_VALUE 1

#define LV_USE_ARC        1  /**< Pentru ace / ceasuri rotunde */
#define LV_USE_BAR        1  /**< Pentru nivel baterie / combustibil */
#define LV_USE_SCALE      1  /**< Pentru gradatii pe ceasuri */
#define LV_USE_LABEL      1  /**< Pentru afisare valori text/numerice */
#define LV_USE_IMAGE      1  /**< Pentru iconite si martori bord */
#define LV_USE_CHART      1  /**< Pentru grafice in timp real */
#define LV_USE_BUTTON     1  /**< Pentru comutare pagini */
#define LV_USE_SPINNER    1  /**< Pentru animatii de incarcare */

/* Dezactivate pentru economie de memorie */
#define LV_USE_ANIMIMG    0
#define LV_USE_CALENDAR   0
#define LV_USE_KEYBOARD   0
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      1
#define LV_USE_WIN        1
#define LV_USE_LED        0
#define LV_USE_SPINBOX    1
#define LV_USE_OBSERVER   1

/*==================
 * THEMES & LAYOUTS
 *==================*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 1  /**< 1 pentru Dark Mode pe bord */
#endif

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
 * 3RD PARTY LIBRARIES
 *====================*/
#define LV_USE_LODEPNG  1
#define LV_USE_TJPGD    0
#define LV_USE_SDL      1

/* === Driver pentru sistemul de fișiere (STDIO pentru Windows / MinGW) === */
#define LV_USE_FS_STDIO 1
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER 'C'
    #define LV_FS_STDIO_PATH "C:"
    #define LV_FS_STDIO_CACHE_SIZE 0
#endif

/* Dezactivare completă a tuturor celorlalte drivere FS */
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0
#define LV_USE_FS_LITTLEFS 0
#define LV_USE_FS_MEMFS 0
#define LV_USE_FS_FROGFS 0

#endif /* LV_CONF_H */
#endif /* Enable content */