#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

#include <stdlib.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdint.h>
#include <mpfr.h>

#include "coords.h"
#include "mdzfileio.h"
#include "random_palette.h"

#define MIN_PRECISION       1
#define MAX_PRECISION       99999999

#define MIN_DEPTH           1
#define MAX_DEPTH           INT32_MAX

#define DEFAULT_WIDTH       480
#define DEFAULT_HEIGHT      360
#define MAX_WIDTH           32767
#define MAX_HEIGHT          32767

#define DEFAULT_AAFACTOR    1
#define MAX_AA              8

#define DEFAULT_ASPECT      (1.0 + 1.0 / (double)3.0)


typedef int32_t depth_t;

/*
    usage of mpfr for deeper precision than long double allows:

    there are two sets of data and functions. both sets have
    almost exactly the same name, except:

    the second set uses the mpfr_t type from the MPFR library.
    these variables are prefixed by p_ to differentiate them
    from their long double cousins.

    the second set of functions are also prefixed by p_.

    the first set do not have the p_ prefix and use the long
    double data type.

*/

typedef struct
{
    mpfr_t  c_re;
    mpfr_t  c_im;

} julia_info;


typedef enum {
    MANDELBROT,
    JULIA
} fractal_type;

extern const char* fractal_str[];

typedef struct image_info
{
    mpfr_t  xmin, xmax, ymax, width;

    /* saved mandel coords.                         */
    /* we need these when  switching back from      */
    /* julia mode                                   */
    mpfr_t  old_cx, old_cy, old_size;

    coords* pcoords; /* used for updating coords */

    depth_t depth;

    int thread_count;
    int draw_lines;

    int*        raw_data;
    guint32*    rgb_data;

    gboolean j_pre;

    GtkWidget* drawing_area;    /* it's handy to keep this around */

    random_palette* rnd_pal;    /* again... it's handy */

    int real_width;             /* real size. differs from user_size */ 
    int real_height;            /* if anti-aliasing is used */
    int user_width;
    int user_height;

    double aspect;

    int aa_factor;

    fractal_type fr_type;       /* mandelbrot || julia */

    double colour_scale;

    union
    {   /* different fractal types' parameters */
        julia_info julia;
    } u;

    gboolean palette_ip;
    gboolean zoom_new_win;

    gboolean using_mpfr;
    gboolean mpfr_init_done;
    mp_prec_t precision;

    void* rth_ptr;

    int lines_drawn;

} image_info;

image_info * image_info_create(fractal_type fr);

void image_info_destroy(image_info* img);

void image_info_set(image_info * img, int w, int h, int aa_factor);

void image_info_clear_image(image_info* img, gboolean raw, gboolean rgb);

void image_info_set_mpfr(image_info* img, gboolean use_mpfr);

void image_info_mpfr_change(image_info * img, mp_prec_t precision);

void image_info_threads_change(image_info* img, int thread_count);

void image_info_reset_view(image_info * img);


void get_center(image_info* img, mpfr_t x, mpfr_t y, mpfr_t mag);

int image_info_save_all(image_info * img, const char* filename);
int image_info_f_save_all(image_info * img, FILE* f);


/* see enum at top of mdzfileio.h for sect_flags */
int image_info_load_all(image_info * img, int sect_flags,
                                            const char* filename);


/* these only work on the 'settings' section of the file...
    save_settings will bail with error (return 0) if the
    'settings' section is not immediately encountered.
*/


int image_info_save_settings(image_info * img, FILE* fd);

int image_info_load_settings(image_info * img, mdzfile*);

/* toggle between m-set & julia-set */
void image_info_switch_fractal(image_info*, int j_real_px, int j_imag_py);

#ifdef WITH_TMZ_CMDLINE
void image_tmz_save_settings(image_info * img, FILE* fd);
#endif

#endif
