/*
    Copyright (c) 2016 Micha Mettke

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1.  The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2.  Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3.  This notice may not be removed or altered from any source distribution.
*/
#ifndef ZR_H_
#define ZR_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
 * ==============================================================
 *
 *                          CONSTANTS
 *
 * ===============================================================
 */
#define ZR_UTF_INVALID 0xFFFD
#define ZR_UTF_SIZE 4
/* describes the number of bytes a glyph consists of*/
#define ZR_INPUT_MAX 16
/* defines the max number of bytes to be added as text input in one frame */
#define ZR_MAX_COLOR_STACK 32
/* Number of temporary configuration color changes that can be stored */
#define ZR_MAX_ATTRIB_STACK 32
/* Number of temporary configuration attribute changes that can be stored */
#define ZR_MAX_FONT_STACK 32
/* Number of temporary configuration user font changes that can be stored */
#define ZR_MAX_FONT_HEIGHT_STACK 32
/* Number of temporary configuration font height changes that can be stored */
#define ZR_MAX_NUMBER_BUFFER 64
#define ZR_MAX_COMBO_EDIT_BUFFER 128
#define ZR_MAX_EVENTS 32
/*
 * ==============================================================
 *
 *                          DEFINES
 *
 * ===============================================================
 */
#define ZR_COMPILE_WITH_FIXED_TYPES 1
/* setting this define to 1 adds header <stdint.h> for fixed sized types
 * if 0 each type has to be set to the correct size*/
#define ZR_COMPILE_WITH_ASSERT 1
/* setting this define to 1 adds header <assert.h> for the assert macro
  IMPORTANT: it also adds the standard library assert so only use it if wanted*/
#define ZR_COMPILE_WITH_VERTEX_BUFFER 1
/* setting this to 1 adds a vertex draw command list backend to this
 library, which allows you to convert queue commands into vertex draw commands.
 If you do not want or need a default backend you can set this flag to zero
 and the module of the library will not be compiled */
#define ZR_COMPILE_WITH_FONT 1
/* setting this to 1 adds the `stb_truetype` and `stb_rect_pack` header
 to this library and provides a default font for font loading and rendering.
 If you already have font handling or do not want to use this font handler
 you can just set this define to zero and the font module will not be compiled
 and the two headers will not be needed. */
#define ZR_DISABLE_STB_RECT_PACK_IMPLEMENTATION 0
/* If you already provide the implementation for stb_rect_pack.h in one of your
 files you have to define this as 1 to prevent another implementation and the
 resulting symbol collision. */
#define ZR_DISABLE_STB_TRUETYPE_IMPLEMENTATION 0
/* If you already provide the implementation for stb_truetype.h in one of your
 files you have to define this as 1 to prevent another implementation and the
 resulting symbol collision. */
#define ZR_COMPILE_WITH_COMMAND_USERDATA 0
/* Activating this adds a userdata pointer into each command */
/*
 * ===============================================================
 *
 *                          BASIC
 *
 * ===============================================================
 */
#if ZR_COMPILE_WITH_FIXED_TYPES
#include <stdint.h>
typedef uint32_t zr_uint;
typedef uint32_t zr_hash;
typedef uintptr_t zr_size;
typedef uintptr_t zr_ptr;
typedef uint32_t zr_flags;
typedef uint32_t zr_rune;
typedef uint8_t zr_byte;
#else
typedef unsigned int zr_uint;
typedef unsigned int zr_hash;
typedef unsigned long zr_size;
typedef zr_size zr_ptr;
typedef unsigned int zr_flags;
typedef unsigned int zr_rune;
typedef unsigned char zr_byte;
#endif

#if ZR_COMPILE_WITH_ASSERT
#ifndef ZR_ASSERT
#include <assert.h>
#define ZR_ASSERT(expr) assert(expr)
#endif
#else
#define ZR_ASSERT(expr)
#endif

struct zr_user_font;
struct zr_edit_box;
struct zr_user_font_glyph;

/* ===============================================================
 *                          UTILITY
 * ===============================================================*/
#define ZR_UNDEFINED (-1.0f)
#define ZR_FLAG(x) (1 << (x))

typedef zr_size zr_element;
enum {zr_false, zr_true};
struct zr_color {zr_byte r,g,b,a;};
struct zr_vec2 {float x,y;};
struct zr_vec2i {short x, y;};
struct zr_rect {float x,y,w,h;};
struct zr_recti {short x,y,w,h;};
typedef char zr_glyph[ZR_UTF_SIZE];
typedef union {void *ptr; int id;} zr_handle;
struct zr_image {zr_handle handle;unsigned short w,h;unsigned short region[4];};
struct zr_scroll {unsigned short x, y;};
enum zr_heading {ZR_UP, ZR_RIGHT, ZR_DOWN, ZR_LEFT};

/* math */
zr_hash zr_murmur_hash(const void *key, int len, zr_hash seed);
void zr_triangle_from_direction(struct zr_vec2 *result, struct zr_rect r,
                                float pad_x, float pad_y, enum zr_heading);

struct zr_vec2 zr_vec2(float x, float y);
struct zr_vec2 zr_vec2i(int x, int y);
struct zr_vec2 zr_vec2v(const float *xy);
struct zr_vec2 zr_vec2iv(const int *xy);

struct zr_rect zr_get_null_rect(void);
struct zr_rect zr_rect(float x, float y, float w, float h);
struct zr_rect zr_recti(int x, int y, int w, int h);
struct zr_rect zr_recta(struct zr_vec2 pos, struct zr_vec2 size);
struct zr_rect zr_rectv(const float *xywh);
struct zr_rect zr_rectiv(const int *xywh);

/* UTF-8 */
zr_size zr_utf_decode(const char*, zr_rune*, zr_size);
zr_size zr_utf_encode(zr_rune, char*, zr_size);
zr_size zr_utf_len(const char*, zr_size byte_len);

/* color (conversion user --> zahnrad) */
struct zr_color zr_rgb(int r, int g, int b);
struct zr_color zr_rgb_iv(const int *rgb);
struct zr_color zr_rgb_bv(const zr_byte* rgb);
struct zr_color zr_rgb_f(float r, float g, float b);
struct zr_color zr_rgb_fv(const float *rgb);
struct zr_color zr_rgb_hex(const char *rgb);

struct zr_color zr_rgba(int r, int g, int b, int a);
struct zr_color zr_rgba_u32(zr_uint);
struct zr_color zr_rgba_iv(const int *rgba);
struct zr_color zr_rgba_bv(const zr_byte *rgba);
struct zr_color zr_rgba_f(float r, float g, float b, float a);
struct zr_color zr_rgba_fv(const float *rgba);
struct zr_color zr_rgba_hex(const char *rgb);

struct zr_color zr_hsv(int h, int s, int v);
struct zr_color zr_hsv_iv(const int *hsv);
struct zr_color zr_hsv_bv(const zr_byte *hsv);
struct zr_color zr_hsv_f(float h, float s, float v);
struct zr_color zr_hsv_fv(const float *hsv);

struct zr_color zr_hsva(int h, int s, int v, int a);
struct zr_color zr_hsva_iv(const int *hsva);
struct zr_color zr_hsva_bv(const zr_byte *hsva);
struct zr_color zr_hsva_f(float h, float s, float v, float a);
struct zr_color zr_hsva_fv(const float *hsva);

/* color (conversion zahnrad --> user) */
void zr_color_f(float *r, float *g, float *b, float *a, struct zr_color);
void zr_color_fv(float *rgba_out, struct zr_color);
zr_uint zr_color_u32(struct zr_color);
void zr_color_hex_rgba(char *output, struct zr_color);
void zr_color_hex_rgb(char *output, struct zr_color);

void zr_color_hsv_i(int *out_h, int *out_s, int *out_v, struct zr_color);
void zr_color_hsv_b(zr_byte *out_h, zr_byte *out_s, zr_byte *out_v, struct zr_color);
void zr_color_hsv_iv(int *hsv_out, struct zr_color);
void zr_color_hsv_bv(zr_byte *hsv_out, struct zr_color);
void zr_color_hsv_f(float *out_h, float *out_s, float *out_v, struct zr_color);
void zr_color_hsv_fv(float *hsv_out, struct zr_color);

void zr_color_hsva_i(int *h, int *s, int *v, int *a, struct zr_color);
void zr_color_hsva_b(zr_byte *h, zr_byte *s, zr_byte *v, zr_byte *a, struct zr_color);
void zr_color_hsva_iv(int *hsva_out, struct zr_color);
void zr_color_hsva_bv(zr_byte *hsva_out, struct zr_color);
void zr_color_hsva_f(float *out_h, float *out_s, float *out_v,
                    float *out_a, struct zr_color);
void zr_color_hsva_fv(float *hsva_out, struct zr_color);

/* image */
zr_handle zr_handle_ptr(void*);
zr_handle zr_handle_id(int);
struct zr_image zr_image_ptr(void*);
struct zr_image zr_image_id(int);
int zr_image_is_subimage(const struct zr_image* img);
struct zr_image zr_subimage_ptr(void*, unsigned short w, unsigned short h,
                                struct zr_rect sub_region);
struct zr_image zr_subimage_id(int, unsigned short w, unsigned short h,
                                struct zr_rect sub_region);

/* ==============================================================
 *
 *                          MEMORY BUFFER
 *
 * ===============================================================*/
/*  A basic (double)-buffer with linear allocation and resetting as only
    freeing policy. The buffers main purpose is to control all memory management
    inside the GUI toolkit and still leave memory control as much as possible in
    the hand of the user. The memory is provided in three different ways.
    The first way is to use a fixed size block of memory to be filled up.
    Biggest advantage is a simple memory model. Downside is that if the buffer
    is full there is no way to accesses more memory, which fits target
    application with a GUI with roughly known memory consumptions.
    The second way to manage memory is by extending the fixed size block by
    querying information from the buffer about the used size and needed size and
    allocate new memory if the buffer is full. While this approach is still
    better than just using a fixed size memory block the reallocation still has
    one invalid frame as consquence since the used memory information is only
    available at the end of the frame which leads to the last way of handling
    memory.
    The last and most complicated way of handling memory is by allocator
    callbacks. The user hereby registers callbacks to be called to allocate and
    free memory if needed. While this solves most allocation problems it causes
    some loss of flow control on the user side.

    USAGE
    ----------------------------
    To instantiate the buffer you either have to call the fixed size or
    allocator initialization function and provide a memory block in the first
    case and an allocator in the second case.
    To allocate memory from the buffer you would call zr_buffer_alloc with a
    request memory block size as well as an alignment for the block.
    Finally to reset the memory at the end of the frame and when the memory
    buffer inside the buffer is no longer needed you would call zr_buffer_reset.
    To free all memory that has been allocated by an allocator if the buffer is
    no longer being used you have to call zr_buffer_clear.
*/
struct zr_memory_status {
    void *memory;
    /* pointer to the currently used memory block inside the referenced buffer*/
    unsigned int type;
    /* type of the buffer which is either fixed size or dynamic */
    zr_size size;
    /* total size of the memory block */
    zr_size allocated;
    /* allocated amount of memory */
    zr_size needed;
    /* memory size that would have been allocated if enough memory was present*/
    zr_size calls;
    /* number of allocation calls referencing this buffer */
};

struct zr_allocator {
    zr_handle userdata;
    void*(*alloc)(zr_handle, zr_size);
    void(*free)(zr_handle, void*);
};

enum zr_allocation_type {
    ZR_BUFFER_FIXED,
    ZR_BUFFER_DYNAMIC
};

enum zr_buffer_allocation_type {
    ZR_BUFFER_FRONT,
    ZR_BUFFER_BACK,
    ZR_BUFFER_MAX
};

struct zr_buffer_marker {
    int active;
    zr_size offset;
};

struct zr_memory {void *ptr;zr_size size;};
struct zr_buffer {
    struct zr_buffer_marker marker[ZR_BUFFER_MAX];
    /* buffer marker to free a buffer to a certain offset */
    struct zr_allocator pool;
    /* allocator callback for dynamic buffers */
    enum zr_allocation_type type;
    /* memory management type */
    struct zr_memory memory;
    /* memory and size of the current memory block */
    float grow_factor;
    /* growing factor for dynamic memory management */
    zr_size allocated;
    /* total amount of memory allocated */
    zr_size needed;
    /* totally consumed memory given that enough memory is present */
    zr_size calls;
    /* number of allocation calls */
    zr_size size;
    /* current size of the buffer */
};

void zr_buffer_init(struct zr_buffer*, const struct zr_allocator*, zr_size size);
void zr_buffer_init_fixed(struct zr_buffer*, void *memory, zr_size size);
void zr_buffer_info(struct zr_memory_status*, struct zr_buffer*);
void zr_buffer_free(struct zr_buffer*);
void *zr_buffer_memory(struct zr_buffer*);
const void *zr_buffer_memory_const(const struct zr_buffer*);
zr_size zr_buffer_total(struct zr_buffer*);

/* ===============================================================
 *
 *                          FONT
 *
 * ===============================================================*/
/*  Font handling in this library can be achived in three different ways.
    The first and simplest ways is by just using your font handling mechanism
    and provide a simple callback for text string width calculation with
    `zr_user_font`. This requires the default drawing output
    and is not possible for the optional vertex buffer output.

    The second way of font handling is by using the same `zr_user_font` struct
    to referencing a font as before but providing a second callback for
    `zr_user_font_glyph` querying which is used for text drawing in the optional
    vertex buffer output. In addition to the callback it is also required to
    provide a texture atlas from the font to draw.

    The final and most complex way is to use the optional font baker
    and font handling function, which requires two additional headers for
    TTF font baking. While the previous two methods did no need any functions
    outside callbacks and are therefore rather simple to handle, the final
    font handling method is quite complex and you need to handle the complex
    font baking API. The reason why it is complex is because there are multible
    ways of using the API. For example it must be possible to use the font
    for default command output as well as vertex buffer output. So for example
    texture coordinates can either be UV for vertex buffer output or absolute
    pixel for drawing function based on pixels. Furthermore it is possible to
    incoperate custom user data into the resulting baked image (for example a
    white pixel for the vertex buffer output).
    In addition and probably the most complex aspect of the baking API was to
    incoperate baking of multible fonts into one image.

    In general the font baking API can be understood as having a number of
    loaded in memory TTF-fonts, font baking configuration and optional custom
    render data as input, while the output is made of font specific data, a big
    glyph array of all baked glyphs and the baked image. The API
    was designed that way to have a typical file format and not
    a perfectly ready in memory library instance of a font. The reason is more
    control and seperates the font baking code from the in library used font
    format.
*/
typedef zr_size(*zr_text_width_f)(zr_handle, float h, const char*, zr_size len);
typedef void(*zr_query_font_glyph_f)(zr_handle handle, float font_height,
                                    struct zr_user_font_glyph *glyph,
                                    zr_rune codepoint, zr_rune next_codepoint);

#if ZR_COMPILE_WITH_VERTEX_BUFFER
struct zr_user_font_glyph {
    struct zr_vec2 uv[2];
    /* texture coordinates */
    struct zr_vec2 offset;
    /* offset between top left and glyph */
    float width, height;
    /* size of the glyph  */
    float xadvance;
    /* offset to the next glyph */
};
#endif

struct zr_user_font {
    zr_handle userdata;
    /* user provided font handle */
    float height;
    /* max height of the font */
    zr_text_width_f width;
    /* font string width in pixel callback */
#if ZR_COMPILE_WITH_VERTEX_BUFFER
    zr_query_font_glyph_f query;
    /* font glyph callback to query drawing info */
    zr_handle texture;
    /* texture handle to the used font atlas or texture */
#endif
};

#ifdef ZR_COMPILE_WITH_FONT
enum zr_font_coord_type {
    ZR_COORD_UV,
    /* texture coordinates inside font glyphs are clamped between 0-1 */
    ZR_COORD_PIXEL
    /* texture coordinates inside font glyphs are in absolute pixel */
};

struct zr_baked_font {
    float height;
    /* height of the font  */
    float ascent, descent;
    /* font glyphs ascent and descent  */
    zr_rune glyph_offset;
    /* glyph array offset inside the font glyph baking output array  */
    zr_rune glyph_count;
    /* number of glyphs of this font inside the glyph baking array output */
    const zr_rune *ranges;
    /* font codepoint ranges as pairs of (from/to) and 0 as last element */
};

struct zr_font_config {
    void *ttf_blob;
    /* pointer to loaded TTF file memory block */
    zr_size ttf_size;
    /* size of the loaded TTF file memory block */
    float size;
    /* baked pixel height of the font */
    unsigned int oversample_h, oversample_v;
    /* rasterize at hight quality for sub-pixel position */
    int pixel_snap;
    /* align very character to pixel boundry (if true set oversample (1,1)) */
    enum zr_font_coord_type coord_type;
    /* texture coordinate format with either pixel or UV coordinates */
    struct zr_vec2 spacing;
    /* extra pixel spacing between glyphs  */
    const zr_rune *range;
    /* list of unicode ranges (2 values per range, zero terminated) */
    struct zr_baked_font *font;
    /* font to setup in the baking process  */
};

struct zr_font_glyph {
    zr_rune codepoint;
    float xadvance;
    float x0, y0, x1, y1, w, h;
    float u0, v0, u1, v1;
};

struct zr_font {
    float size;
    float scale;
    float ascent, descent;
    struct zr_font_glyph *glyphs;
    const struct zr_font_glyph *fallback;
    zr_rune fallback_codepoint;
    zr_rune glyph_count;
    const zr_rune *ranges;
    zr_handle atlas;
};

/* some language glyph codepoint ranges */
const zr_rune *zr_font_default_glyph_ranges(void);
const zr_rune *zr_font_chinese_glyph_ranges(void);
const zr_rune *zr_font_cyrillic_glyph_ranges(void);
const zr_rune *zr_font_korean_glyph_ranges(void);

/* font baking functions (need to be called sequentially top to bottom) */
void zr_font_bake_memory(zr_size *temporary_memory, int *glyph_count,
                            struct zr_font_config*, int count);
int zr_font_bake_pack(zr_size *img_memory, int *img_width, int *img_height,
                            struct zr_recti *custom_space,
                            void *temporary_memory, zr_size temporary_size,
                            const struct zr_font_config*, int font_count);
void zr_font_bake(void *image_memory, int image_width, int image_height,
                    void *temporary_memory, zr_size temporary_memory_size,
                    struct zr_font_glyph*, int glyphs_count,
                    const struct zr_font_config*, int font_count);
void zr_font_bake_custom_data(void *img_memory, int img_width, int img_height,
                            struct zr_recti img_dst, const char *image_data_mask,
                            int tex_width, int tex_height,char white,char black);
void zr_font_bake_convert(void *out_memory, int image_width, int image_height,
                            const void *in_memory);
void zr_font_init(struct zr_font*, float pixel_height, zr_rune fallback_codepoint,
                    struct zr_font_glyph*, const struct zr_baked_font*,
                    zr_handle atlas);
struct zr_user_font zr_font_ref(struct zr_font*);
const struct zr_font_glyph* zr_font_find_glyph(struct zr_font*, zr_rune unicode);

#endif

/*===============================================================
 *                          EDIT BOX
 * ===============================================================*/
typedef int(*zr_filter)(const struct zr_edit_box*, zr_rune unicode);
typedef void(*zr_paste_f)(zr_handle, struct zr_edit_box*);
typedef void(*zr_copy_f)(zr_handle, const char*, zr_size size);

/* filter function */
struct zr_edit_box;
int zr_filter_default(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_ascii(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_float(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_decimal(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_hex(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_oct(const struct zr_edit_box*, zr_rune unicode);
int zr_filter_binary(const struct zr_edit_box*, zr_rune unicode);

enum zr_edit_remove_operation {
    ZR_DELETE = 0,
    ZR_REMOVE
};

enum zr_edit_flags {
    ZR_EDIT_READ_ONLY   = ZR_FLAG(0),
    /* text inside the edit widget cannot be modified */
    ZR_EDIT_CURSOR      = ZR_FLAG(1),
    /* edit widget will have a movable cursor */
    ZR_EDIT_SELECTABLE  = ZR_FLAG(2),
    /* edit widget allows text selection */
    ZR_EDIT_CLIPBOARD   = ZR_FLAG(3),
    /* edit widget tries to use the clipbard callback for copy & paste */
    ZR_EDIT_SIGCOMIT    = ZR_FLAG(4)
    /* edit widget generateds ZR_EDIT_COMMITED event on enter */
};

enum zr_edit_types {
    ZR_EDIT_SIMPLE = 0,
    ZR_EDIT_FIELD = (ZR_EDIT_CURSOR|ZR_EDIT_SELECTABLE|ZR_EDIT_CLIPBOARD),
    ZR_EDIT_BOX = (ZR_EDIT_CURSOR|ZR_EDIT_SELECTABLE| ZR_EDIT_CLIPBOARD)
};

enum zr_edit_events {
    ZR_EDIT_ACTIVE      = ZR_FLAG(0),
    /* edit widget is currently being modified */
    ZR_EDIT_INACTIVE    = ZR_FLAG(1),
    /* edit widget is not active and is not being modified */
    ZR_EDIT_ACTIVATED   = ZR_FLAG(2),
    /* edit widget went from state inactive to state active */
    ZR_EDIT_DEACTIVATED = ZR_FLAG(3),
    /* edit widget went from state active to state inactive */
    ZR_EDIT_COMMITED    = ZR_FLAG(4)
    /* edit widget has received an enter and lost focus */
};

struct zr_text_selection {
    int active;
    zr_size begin;
    zr_size end;
};

struct zr_clipboard {
    zr_handle userdata;
    zr_paste_f paste;
    zr_copy_f copy;
};

struct zr_edit_box {
    struct zr_buffer buffer;
    int active;
    zr_size cursor;
    zr_size glyphs;
    struct zr_clipboard clip;
    zr_filter filter;
    struct zr_text_selection sel;
    float scrollbar;
    int text_inserted;
};

/* editbox */
void zr_edit_box_clear(struct zr_edit_box*);
void zr_edit_box_add(struct zr_edit_box*, const char*, zr_size);
void zr_edit_box_remove(struct zr_edit_box*, enum zr_edit_remove_operation);
char *zr_edit_box_get(struct zr_edit_box*);
const char *zr_edit_box_get_const(const struct zr_edit_box*);
void zr_edit_box_at(struct zr_edit_box*, zr_size pos, zr_glyph, zr_size*);
void zr_edit_box_at_cursor(struct zr_edit_box*, zr_glyph, zr_size*);
char zr_edit_box_at_char(struct zr_edit_box*, zr_size pos);
void zr_edit_box_set_cursor(struct zr_edit_box*, zr_size pos);
zr_size zr_edit_box_get_cursor(struct zr_edit_box *eb);
zr_size zr_edit_box_len_char(struct zr_edit_box*);
zr_size zr_edit_box_len(struct zr_edit_box*);
int zr_edit_box_has_selection(const struct zr_edit_box*);
const char *zr_edit_box_get_selection(zr_size *len, struct zr_edit_box*);

/* ===============================================================
 *
 *                          RENDERING
 *
 * ===============================================================*/
/*  This library was designed to be render backend agnostic so it does
    not draw anything to the screen. Instead all drawn primitives, widgets
    are made of, are buffered into memory and make up a command queue.
    Each frame therefore fills the command buffer with draw commands
    that then need to be executed by the user and his own render backend.
    After that the command buffer needs to be cleared and a new frame can be
    started.

    The reason for buffering simple primitives as draw commands instead of
    directly buffering a hardware accessible format with vertex and element
    buffer was to support native render backends like X11 and Win32.
    That being said it is possible to convert the command buffer into a
    hardware accessible format to support hardware based rendering as well.
*/
enum zr_command_type {
    ZR_COMMAND_NOP,
    ZR_COMMAND_SCISSOR,
    ZR_COMMAND_LINE,
    ZR_COMMAND_CURVE,
    ZR_COMMAND_RECT,
    ZR_COMMAND_RECT_FILLED,
    ZR_COMMAND_RECT_MULTI_COLOR,
    ZR_COMMAND_CIRCLE,
    ZR_COMMAND_CIRCLE_FILLED,
    ZR_COMMAND_ARC,
    ZR_COMMAND_ARC_FILLED,
    ZR_COMMAND_TRIANGLE,
    ZR_COMMAND_TRIANGLE_FILLED,
    ZR_COMMAND_POLYGON,
    ZR_COMMAND_POLYGON_FILLED,
    ZR_COMMAND_POLYLINE,
    ZR_COMMAND_TEXT,
    ZR_COMMAND_IMAGE
};

/* command base and header of every comand inside the buffer */
struct zr_command {
    enum zr_command_type type;
    zr_size next;
#if ZR_COMPILE_WITH_COMMAND_USERDATA
    zr_handle userdata;
#endif
};

struct zr_command_scissor {
    struct zr_command header;
    short x, y;
    unsigned short w, h;
};

struct zr_command_line {
    struct zr_command header;
    unsigned short line_thickness;
    struct zr_vec2i begin;
    struct zr_vec2i end;
    struct zr_color color;
};

struct zr_command_curve {
    struct zr_command header;
    unsigned short line_thickness;
    struct zr_vec2i begin;
    struct zr_vec2i end;
    struct zr_vec2i ctrl[2];
    struct zr_color color;
};

struct zr_command_rect {
    struct zr_command header;
    unsigned short rounding;
    unsigned short line_thickness;
    short x, y;
    unsigned short w, h;
    struct zr_color color;
};

struct zr_command_rect_filled {
    struct zr_command header;
    unsigned short rounding;
    short x, y;
    unsigned short w, h;
    struct zr_color color;
};

struct zr_command_rect_multi_color {
    struct zr_command header;
    short x, y;
    unsigned short w, h;
    struct zr_color left;
    struct zr_color top;
    struct zr_color bottom;
    struct zr_color right;
};

struct zr_command_triangle {
    struct zr_command header;
    unsigned short line_thickness;
    struct zr_vec2i a;
    struct zr_vec2i b;
    struct zr_vec2i c;
    struct zr_color color;
};

struct zr_command_triangle_filled {
    struct zr_command header;
    struct zr_vec2i a;
    struct zr_vec2i b;
    struct zr_vec2i c;
    struct zr_color color;
};

struct zr_command_circle {
    struct zr_command header;
    short x, y;
    unsigned short line_thickness;
    unsigned short w, h;
    struct zr_color color;
};

struct zr_command_circle_filled {
    struct zr_command header;
    short x, y;
    unsigned short w, h;
    struct zr_color color;
};

struct zr_command_arc {
    struct zr_command header;
    short cx, cy;
    unsigned short r;
    unsigned short line_thickness;
    float a[2];
    struct zr_color color;
};

struct zr_command_arc_filled {
    struct zr_command header;
    short cx, cy;
    unsigned short r;
    float a[2];
    struct zr_color color;
};

struct zr_command_polygon {
    struct zr_command header;
    struct zr_color color;
    unsigned short line_thickness;
    unsigned short point_count;
    struct zr_vec2i points[1];
};

struct zr_command_polygon_filled {
    struct zr_command header;
    struct zr_color color;
    unsigned short point_count;
    struct zr_vec2i points[1];
};

struct zr_command_polyline {
    struct zr_command header;
    struct zr_color color;
    unsigned short line_thickness;
    unsigned short point_count;
    struct zr_vec2i points[1];
};

struct zr_command_image {
    struct zr_command header;
    short x, y;
    unsigned short w, h;
    struct zr_image img;
};

struct zr_command_text {
    struct zr_command header;
    const struct zr_user_font *font;
    struct zr_color background;
    struct zr_color foreground;
    short x, y;
    unsigned short w, h;
    float height;
    zr_size length;
    char string[1];
};

enum zr_command_clipping {
    ZR_CLIPPING_OFF = zr_false,
    ZR_CLIPPING_ON = zr_true
};

struct zr_command_buffer {
    struct zr_buffer *base;
    struct zr_rect clip;
    int use_clipping;
    zr_handle userdata;
    zr_size begin, end, last;
};

#if ZR_COMPILE_WITH_VERTEX_BUFFER
typedef unsigned short zr_draw_index;
typedef zr_uint zr_draw_vertex_color;

enum zr_anti_aliasing {
    ZR_ANTI_ALIASING_OFF = zr_false,
    ZR_ANTI_ALIASING_ON
};

struct zr_draw_vertex {
    struct zr_vec2 position;
    struct zr_vec2 uv;
    zr_draw_vertex_color col;
};
#endif

struct zr_draw_command {
    unsigned int elem_count;
    /* number of elements in the current draw batch */
    struct zr_rect clip_rect;
    /* current screen clipping rectangle */
    zr_handle texture;
    /* current texture to set */
#if ZR_COMPILE_WITH_COMMAND_USERDATA
    zr_handle userdata;
#endif
};

struct zr_draw_null_texture {
    zr_handle texture;
    /* texture handle to a texture with a white pixel */
    struct zr_vec2 uv;
    /* coordinates to a white pixel in the texture  */
};

/* stroking routines */
void zr_push_scissor(struct zr_command_buffer*, struct zr_rect);
void zr_stroke_line(struct zr_command_buffer *b, float x0, float y0,
                    float x1, float y1, float line_thickness, struct zr_color);
void zr_stroke_curve(struct zr_command_buffer*, float, float, float, float,
                    float, float, float, float, float line_thickness, struct zr_color);
void zr_stroke_rect(struct zr_command_buffer*, struct zr_rect, float rounding,
                    float line_thickness, struct zr_color);
void zr_stroke_circle(struct zr_command_buffer*, struct zr_rect, float line_thickness,
                    struct zr_color);
void zr_stroke_arc(struct zr_command_buffer*, float cx, float cy, float radius,
                    float a_min, float a_max, float line_thickness, struct zr_color);
void zr_stroke_triangle(struct zr_command_buffer*, float, float, float, float,
                        float, float, float line_thichness, struct zr_color);
void zr_stroke_polyline(struct zr_command_buffer*, float *points, int point_count,
                        struct zr_color col);
void zr_stroke_polygon(struct zr_command_buffer*, float*, int point_count,
                    float line_thickness, struct zr_color);

/* filling routines */
void zr_fill_rect(struct zr_command_buffer*, struct zr_rect, float rounding,
                        struct zr_color);
void zr_fill_rect_multi_color(struct zr_command_buffer*, struct zr_rect,
                                struct zr_color left, struct zr_color top,
                                struct zr_color right, struct zr_color bottom);
void zr_fill_circle(struct zr_command_buffer*, struct zr_rect, struct zr_color);
void zr_fill_arc(struct zr_command_buffer*, float cx, float cy, float radius,
                        float a_min, float a_max, struct zr_color);
void zr_fill_triangle(struct zr_command_buffer*, float x0, float y0,
                            float x1, float y1, float x2, float y2, struct zr_color);
void zr_fill_polygon(struct zr_command_buffer*, float*, int point_count, struct zr_color);
void zr_draw_image(struct zr_command_buffer*, struct zr_rect, const struct zr_image*);
void zr_draw_text(struct zr_command_buffer*, struct zr_rect,
                    const char *text, zr_size len, const struct zr_user_font*,
                    struct zr_color, struct zr_color);

/* ===============================================================
 *
 *                          GUI
 *
 * ===============================================================*/
enum zr_keys {
    ZR_KEY_SHIFT,
    ZR_KEY_DEL,
    ZR_KEY_ENTER,
    ZR_KEY_TAB,
    ZR_KEY_BACKSPACE,
    ZR_KEY_COPY,
    ZR_KEY_CUT,
    ZR_KEY_PASTE,
    ZR_KEY_UP,
    ZR_KEY_DOWN,
    ZR_KEY_LEFT,
    ZR_KEY_RIGHT,
    ZR_KEY_MAX
};

enum zr_buttons {
    ZR_BUTTON_LEFT,
    ZR_BUTTON_MIDDLE,
    ZR_BUTTON_RIGHT,
    ZR_BUTTON_MAX
};

struct zr_mouse_button {
    int down;
    unsigned int clicked;
    struct zr_vec2 clicked_pos;
};

struct zr_mouse {
    struct zr_mouse_button buttons[ZR_BUTTON_MAX];
    struct zr_vec2 pos;
    struct zr_vec2 prev;
    struct zr_vec2 delta;
    float scroll_delta;
};

struct zr_key {
    int down;
    unsigned int clicked;
};

struct zr_keyboard {
    struct zr_key keys[ZR_KEY_MAX];
    char text[ZR_INPUT_MAX];
    zr_size text_len;
};

struct zr_input {
    struct zr_keyboard keyboard;
    struct zr_mouse mouse;
};

int zr_input_has_mouse_click_in_rect(const struct zr_input*,
                                    enum zr_buttons, struct zr_rect);
int zr_input_has_mouse_click_down_in_rect(const struct zr_input*, enum zr_buttons,
                                        struct zr_rect, int down);
int zr_input_is_mouse_click_in_rect(const struct zr_input*,
                                    enum zr_buttons, struct zr_rect);
int zr_input_is_mouse_click_down_in_rect(const struct zr_input *i, enum zr_buttons id,
                                        struct zr_rect b, int down);
int zr_input_any_mouse_click_in_rect(const struct zr_input*, struct zr_rect);
int zr_input_is_mouse_prev_hovering_rect(const struct zr_input*, struct zr_rect);
int zr_input_is_mouse_hovering_rect(const struct zr_input*, struct zr_rect);
int zr_input_mouse_clicked(const struct zr_input*, enum zr_buttons, struct zr_rect);
int zr_input_is_mouse_down(const struct zr_input*, enum zr_buttons);
int zr_input_is_mouse_pressed(const struct zr_input*, enum zr_buttons);
int zr_input_is_mouse_released(const struct zr_input*, enum zr_buttons);
int zr_input_is_key_pressed(const struct zr_input*, enum zr_keys);
int zr_input_is_key_released(const struct zr_input*, enum zr_keys);
int zr_input_is_key_down(const struct zr_input*, enum zr_keys);

/* ==============================================================
 *                          STYLE
 * ===============================================================*/
enum zr_symbol_type {
    ZR_SYMBOL_NONE,
    ZR_SYMBOL_X,
    ZR_SYMBOL_UNDERSCORE,
    ZR_SYMBOL_CIRCLE,
    ZR_SYMBOL_CIRCLE_FILLED,
    ZR_SYMBOL_RECT,
    ZR_SYMBOL_RECT_FILLED,
    ZR_SYMBOL_TRIANGLE_UP,
    ZR_SYMBOL_TRIANGLE_DOWN,
    ZR_SYMBOL_TRIANGLE_LEFT,
    ZR_SYMBOL_TRIANGLE_RIGHT,
    ZR_SYMBOL_PLUS,
    ZR_SYMBOL_MINUS,
    ZR_SYMBOL_MAX
};

enum zr_style_item_type {
    ZR_STYLE_ITEM_COLOR,
    ZR_STYLE_ITEM_IMAGE
};

union zr_style_item_data {
    struct zr_image image;
    struct zr_color color;
};

struct zr_style_item {
    enum zr_style_item_type type;
    union zr_style_item_data data;
};

struct zr_style_text {
    struct zr_color color;
    struct zr_vec2 padding;
};

struct zr_style_button;
struct zr_style_custom_button_drawing {
    void(*button_text)(struct zr_command_buffer*,
        const struct zr_rect *background, const struct zr_rect*,
        zr_flags state, const struct zr_style_button*,
        const char*, zr_size len, zr_flags text_alignment,
        const struct zr_user_font*);
    void(*button_symbol)(struct zr_command_buffer*,
        const struct zr_rect *background, const struct zr_rect*,
        zr_flags state, const struct zr_style_button*,
        enum zr_symbol_type, const struct zr_user_font*);
    void(*button_image)(struct zr_command_buffer*,
        const struct zr_rect *background, const struct zr_rect*,
        zr_flags state, const struct zr_style_button*,
        const struct zr_image *img);
    void(*button_text_symbol)(struct zr_command_buffer*,
        const struct zr_rect *background, const struct zr_rect*,
        const struct zr_rect *symbol, zr_flags state,
        const struct zr_style_button*,
        const char *text, zr_size len, zr_flags align,
        enum zr_symbol_type, const struct zr_user_font*);
    void(*button_text_image)(struct zr_command_buffer*,
        const struct zr_rect *background, const struct zr_rect*,
        const struct zr_rect *image, zr_flags state,
        const struct zr_style_button*,
        const char *text, zr_size len, zr_flags align, const struct zr_user_font*,
        const struct zr_image *img);
};

struct zr_style_button {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* text */
    struct zr_color text_background;
    struct zr_color text_normal;
    struct zr_color text_hover;
    struct zr_color text_active;
    zr_flags text_alignment;

    /* properties */
    float border;
    float rounding;
    float fixed_width;
    float fixed_height;
    int has_fixed_size;
    struct zr_vec2 padding;
    struct zr_vec2 image_padding;
    struct zr_vec2 touch_padding;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle userdata);
    struct zr_style_custom_button_drawing draw;
    void(*draw_end)(zr_handle, struct zr_command_buffer*, zr_handle userdata);
};

struct zr_style_toggle;
union zr_style_custom_toggle_drawing {
    void(*radio)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_toggle *toggle, int active,
        const struct zr_rect *label, const struct zr_rect *selector,
        const struct zr_rect *cursor, const char *string, zr_size len,
        const struct zr_user_font *font);
    void(*checkbox)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_toggle *toggle, int active,
        const struct zr_rect *label, const struct zr_rect *selector,
        const struct zr_rect *cursor, const char *string, zr_size len,
        const struct zr_user_font *font);
};

struct zr_style_toggle {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;

    /* cursor */
    struct zr_style_item cursor_normal;
    struct zr_style_item cursor_hover;

    /* text */
    struct zr_color text_normal;
    struct zr_color text_hover;
    struct zr_color text_active;
    struct zr_color text_background;
    zr_flags text_alignment;

    /* properties */
    float fixed_width;
    float fixed_height;
    int has_fixed_size;
    struct zr_vec2 padding;
    struct zr_vec2 touch_padding;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    union zr_style_custom_toggle_drawing draw;
    void(*draw_end)(struct zr_command_buffer*, zr_handle);
};

struct zr_style_selectable {
    /* background (inactive) */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item pressed;

    /* background (active) */
    struct zr_style_item normal_active;
    struct zr_style_item hover_active;
    struct zr_style_item pressed_active;

    /* text color (inactive) */
    struct zr_color text_normal;
    struct zr_color text_hover;
    struct zr_color text_pressed;

    /* text color (active) */
    struct zr_color text_normal_active;
    struct zr_color text_hover_active;
    struct zr_color text_pressed_active;
    struct zr_color text_background;
    zr_flags text_alignment;

    /* properties */
    float fixed_width;
    float fixed_height;
    float rounding;
    int has_fixed_size;
    struct zr_vec2 padding;
    struct zr_vec2 touch_padding;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    void(*draw)(struct zr_command_buffer*,
        zr_flags state, const struct zr_style_selectable*, int active,
        const struct zr_rect*, const char *string, zr_size len,
        zr_flags align, const struct zr_user_font*);
    void(*draw_end)(struct zr_command_buffer*, zr_handle);
};

struct zr_style_slider {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* background bar */
    struct zr_color bar_normal;
    struct zr_color bar_hover;
    struct zr_color bar_active;
    struct zr_color bar_filled;

    /* cursor */
    struct zr_style_item cursor_normal;
    struct zr_style_item cursor_hover;
    struct zr_style_item cursor_active;

    /* properties */
    float border;
    float rounding;
    float bar_height;
    struct zr_vec2 padding;
    struct zr_vec2 spacing;
    struct zr_vec2 cursor_size;

    int has_fixed_size;
    float fixed_width;
    float fixed_height;

    /* optional buttons */
    int show_buttons;
    struct zr_style_button inc_button;
    struct zr_style_button dec_button;
    enum zr_symbol_type inc_symbol;
    enum zr_symbol_type dec_symbol;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    void(*draw)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_slider*, const struct zr_rect *bar,
        const struct zr_rect *cursor, float min, float value, float max);
    void(*draw_end)(struct zr_command_buffer*);
};

struct zr_style_progress {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;

    /* cursor */
    struct zr_style_item cursor_normal;
    struct zr_style_item cursor_hover;
    struct zr_style_item cursor_active;

    /* properties */
    float rounding;
    float fixed_width;
    float fixed_height;
    int has_fixed_size;
    struct zr_vec2 padding;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    void(*draw)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_progress*, const struct zr_rect *bounds,
        const struct zr_rect *cursor, zr_size value, zr_size max);
    void(*draw_end)(struct zr_command_buffer*);
};

struct zr_style_scrollbar {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* cursor */
    struct zr_style_item cursor_normal;
    struct zr_style_item cursor_hover;
    struct zr_style_item cursor_active;

    /* properties */
    float border;
    float rounding;
    struct zr_vec2 padding;

    /* optional buttons */
    int show_buttons;
    struct zr_style_button inc_button;
    struct zr_style_button dec_button;
    enum zr_symbol_type inc_symbol;
    enum zr_symbol_type dec_symbol;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    void(*draw)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_scrollbar*, const struct zr_rect *scroll,
        const struct zr_rect *cursor);
    void(*draw_end)(struct zr_command_buffer*, zr_handle);
};

struct zr_style_edit {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* cursor */
    struct zr_style_item cursor_normal;
    struct zr_style_item cursor_hover;
    struct zr_style_item cursor_active;

    /* text (unselected) */
    struct zr_color text_normal;
    struct zr_color text_hover;
    struct zr_color text_active;

    /* text (selected) */
    struct zr_color selected_normal;
    struct zr_color selected_hover;
    struct zr_color selected_text_normal;
    struct zr_color selected_text_hover;

    /* properties */
    float border;
    float rounding;
    float cursor_size;
    float fixed_width;
    float fixed_height;
    int has_fixed_size;
    struct zr_vec2 padding;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*,zr_handle);
    void(*draw)(struct zr_command_buffer*, zr_flags state,
        const struct zr_style_edit*, const struct zr_rect *bounds,
        const struct zr_rect *label, const struct zr_rect *selection,
        int show_cursor, const char *unselected_text, zr_size unselected_len,
        const char *selected_text, zr_size selected_len,
        const struct zr_edit_box*, const struct zr_user_font*);
    void(*draw_end)(struct zr_command_buffer*,zr_handle);
};

struct zr_style_property {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* text */
    struct zr_color label_normal;
    struct zr_color label_hover;
    struct zr_color label_active;

    /* symbols */
    enum zr_symbol_type sym_left;
    enum zr_symbol_type sym_right;

    /* properties */
    float border;
    float rounding;
    int has_fixed_size;
    float fixed_width;
    float fixed_height;
    struct zr_vec2 padding;

    struct zr_style_edit edit;
    struct zr_style_button inc_button;
    struct zr_style_button dec_button;

    /* optional user callbacks */
    zr_handle userdata;
    void(*draw_begin)(struct zr_command_buffer*, zr_handle);
    void(*draw)(struct zr_command_buffer*, const struct zr_style_property*,
        const struct zr_rect*, const struct zr_rect *label, zr_flags state,
        const char *name, zr_size len, const struct zr_user_font*);
    void(*draw_end)(struct zr_command_buffer*, zr_handle);
};

struct zr_style_chart {
    /* colors */
    struct zr_style_item background;
    struct zr_color border_color;
    struct zr_color selected_color;
    struct zr_color color;

    /* properties */
    float border;
    float rounding;
    int has_fixed_size;
    float fixed_width;
    float fixed_height;
    struct zr_vec2 padding;
};

struct zr_style_combo {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;
    struct zr_color border_color;

    /* label */
    struct zr_color label_normal;
    struct zr_color label_hover;
    struct zr_color label_active;

    /* symbol */
    struct zr_color symbol_normal;
    struct zr_color symbol_hover;
    struct zr_color symbol_active;

    /* button */
    struct zr_style_button button;
    enum zr_symbol_type sym_normal;
    enum zr_symbol_type sym_hover;
    enum zr_symbol_type sym_active;

    /* properties */
    float border;
    float rounding;
    int has_fixed_size;
    float fixed_width;
    float fixed_height;
    struct zr_vec2 content_padding;
    struct zr_vec2 button_padding;
    struct zr_vec2 spacing;
};

struct zr_style_tab {
    struct zr_style_item background;
    struct zr_color border_color;
    struct zr_color text;

    struct zr_style_button tab_button;
    struct zr_style_button node_button;
    enum zr_symbol_type sym_minimize;
    enum zr_symbol_type sym_maximize;

    float border;
    float rounding;
    struct zr_vec2 padding;
    struct zr_vec2 spacing;
};

enum zr_style_header_align {
    ZR_HEADER_LEFT,
    ZR_HEADER_RIGHT
};

struct zr_style_window_header {
    /* background */
    struct zr_style_item normal;
    struct zr_style_item hover;
    struct zr_style_item active;

    /* button */
    struct zr_style_button close_button;
    struct zr_style_button minimize_button;
    enum zr_symbol_type close_symbol;
    enum zr_symbol_type minimize_symbol;
    enum zr_symbol_type maximize_symbol;

    /* title */
    struct zr_color label_normal;
    struct zr_color label_hover;
    struct zr_color label_active;

    /* properties */
    enum zr_style_header_align align;
    struct zr_vec2 padding;
    struct zr_vec2 label_padding;
    struct zr_vec2 spacing;
};

struct zr_style_window {
    struct zr_style_window_header header;
    struct zr_style_item fixed_background;
    struct zr_color background;
    struct zr_color border_color;

    struct zr_style_item scaler;
    struct zr_vec2 footer_padding;

    float border;
    float rounding;
    int has_fixed_size;
    float fixed_width;
    float fixed_height;
    struct zr_vec2 scaler_size;
    struct zr_vec2 padding;
    struct zr_vec2 spacing;
    struct zr_vec2 scrollbar_size;
    struct zr_vec2 min_size;
};

struct zr_style {
    struct zr_user_font font;
    struct zr_style_text text;
    struct zr_style_button button;
    struct zr_style_button contextual_button;
    struct zr_style_button menu_button;
    struct zr_style_toggle option;
    struct zr_style_toggle checkbox;
    struct zr_style_selectable selectable;
    struct zr_style_slider slider;
    struct zr_style_progress progress;
    struct zr_style_property property;
    struct zr_style_edit edit;
    struct zr_style_chart line_chart;
    struct zr_style_chart column_chart;
    struct zr_style_scrollbar scrollh;
    struct zr_style_scrollbar scrollv;
    struct zr_style_tab tab;
    struct zr_style_combo combo;
    struct zr_style_window window;
};

struct zr_style_item zr_style_item_image(struct zr_image img);
struct zr_style_item zr_style_item_color(struct zr_color);
struct zr_style_item zr_style_item_hide(void);

/*==============================================================
 *                          PANEL
 * =============================================================*/
enum zr_modify {
    ZR_FIXED = zr_false,
    ZR_MODIFIABLE = zr_true
};

enum zr_orientation {
    ZR_VERTICAL,
    ZR_HORIZONTAL
};

enum zr_collapse_states {
    ZR_MINIMIZED = zr_false,
    ZR_MAXIMIZED = zr_true
};

enum zr_show_states {
    ZR_HIDDEN = zr_false,
    ZR_SHOWN = zr_true
};

/* widget states */
enum zr_widget_states {
    ZR_WIDGET_STATE_INACTIVE  = ZR_FLAG(0),
    /* widget is neither active nor hovered */
    ZR_WIDGET_STATE_ENTERED   = ZR_FLAG(1),
    /* widget has been hovered on the current frame */
    ZR_WIDGET_STATE_HOVERED   = ZR_FLAG(2),
    /* widget is being hovered */
    ZR_WIDGET_STATE_LEFT      = ZR_FLAG(3),
    /* widget is from this frame on not hovered anymore */
    ZR_WIDGET_STATE_ACTIVE    = ZR_FLAG(4)
    /* widget is currently activated */
};

enum zr_widget_layout_states {
    ZR_WIDGET_INVALID,
    /* The widget cannot be seen and is completly out of view */
    ZR_WIDGET_VALID,
    /* The widget is completly inside the window and can be updated and drawn */
    ZR_WIDGET_ROM
    /* The widget is partially visible and cannot be updated */
};

/* text alignment */
enum zr_text_align {
    ZR_TEXT_ALIGN_LEFT        = 0x01,
    ZR_TEXT_ALIGN_CENTERED    = 0x02,
    ZR_TEXT_ALIGN_RIGHT       = 0x04,
    ZR_TEXT_ALIGN_TOP         = 0x08,
    ZR_TEXT_ALIGN_MIDDLE      = 0x10,
    ZR_TEXT_ALIGN_BOTTOM      = 0x20
};
enum zr_text_alignment {
    ZR_TEXT_CENTERED    = ZR_TEXT_ALIGN_MIDDLE|ZR_TEXT_ALIGN_CENTERED,
    ZR_TEXT_LEFT        = ZR_TEXT_ALIGN_MIDDLE|ZR_TEXT_ALIGN_LEFT,
    ZR_TEXT_RIGHT       = ZR_TEXT_ALIGN_MIDDLE|ZR_TEXT_ALIGN_RIGHT
};

enum zr_button_behavior {
    ZR_BUTTON_DEFAULT,
    ZR_BUTTON_REPEATER
};

enum zr_chart_type {
    ZR_CHART_LINES,
    ZR_CHART_COLUMN,
    ZR_CHART_MAX
};

enum zr_chart_event {
    ZR_CHART_HOVERING   = 0x01,
    ZR_CHART_CLICKED    = 0x02
};

enum zr_color_picker_format {
    ZR_RGB  = 0,
    ZR_RGBA = 1
};

enum zr_popup_type {
    ZR_POPUP_STATIC,
    ZR_POPUP_DYNAMIC
};

enum zr_layout_format {
    ZR_DYNAMIC,
    ZR_STATIC
};

enum zr_layout_node_type {
    ZR_LAYOUT_NODE,
    ZR_LAYOUT_TAB
};

struct zr_chart {
    const struct zr_style_chart *style;
    enum zr_chart_type type;
    float x, y, w, h;
    float min, max, range;
    struct zr_vec2 last;
    int index;
    int count;
};

enum zr_panel_flags {
    ZR_WINDOW_BORDER        = ZR_FLAG(0),
    /* Draws a border around the window to visually seperate the window
     * from the background */
    ZR_WINDOW_BORDER_HEADER = ZR_FLAG(1),
    /* Draws a border between window header and body */
    ZR_WINDOW_MOVABLE       = ZR_FLAG(2),
    /* The moveable flag inidicates that a window can be moved by user input or
     * by dragging the window header */
    ZR_WINDOW_SCALABLE      = ZR_FLAG(3),
    /* The scaleable flag indicates that a window can be scaled by user input
     * by dragging a scaler icon at the button of the window */
    ZR_WINDOW_CLOSABLE      = ZR_FLAG(4),
    /* adds a closeable icon into the header */
    ZR_WINDOW_MINIMIZABLE   = ZR_FLAG(5),
    /* adds a minimize icon into the header */
    ZR_WINDOW_DYNAMIC       = ZR_FLAG(6),
    /* special type of window which grows up in height while being filled to a
     * certain maximum height. It is mainly used for combo boxes/menus but can
     * be used to create perfectly fitting windows as well */
    ZR_WINDOW_NO_SCROLLBAR  = ZR_FLAG(7),
    /* Removes the scrollbar from the window */
    ZR_WINDOW_TITLE         = ZR_FLAG(8)
    /* Forces a header at the top at the window showing the title */
};

struct zr_row_layout {
    int type;
    int index;
    float height;
    int columns;
    const float *ratio;
    float item_width, item_height;
    float item_offset;
    float filled;
    struct zr_rect item;
    int tree_depth;
};

struct zr_popup_buffer {
    zr_size begin;
    zr_size parent;
    zr_size last;
    zr_size end;
    int active;
};

struct zr_menu_state {
    float x, y, w, h;
    struct zr_scroll offset;
};

struct zr_panel {
    zr_flags flags;
    struct zr_rect bounds;
    struct zr_scroll *offset;
    float at_x, at_y, max_x;
    float width, height;
    float footer_h;
    float header_h;
    struct zr_rect clip;
    struct zr_menu_state menu;
    struct zr_row_layout row;
    struct zr_chart chart;
    struct zr_popup_buffer popup_buffer;
    struct zr_command_buffer *buffer;
    struct zr_panel *parent;
};

/*==============================================================
 *                          WINDOW
 * =============================================================*/
enum zr_window_flags {
    ZR_WINDOW_PRIVATE       = ZR_FLAG(9),
    /* dummy flag which mark the beginning of the private window flag part */
    ZR_WINDOW_ROM           = ZR_FLAG(10),
    /* sets the window into a read only mode and does not allow input changes */
    ZR_WINDOW_HIDDEN        = ZR_FLAG(11),
    /* Hiddes the window and stops any window interaction and drawing can be set
     * by user input or by closing the window */
    ZR_WINDOW_MINIMIZED     = ZR_FLAG(12),
    /* marks the window as minimized */
    ZR_WINDOW_SUB           = ZR_FLAG(13),
    /* Marks the window as subwindow of another window*/
    ZR_WINDOW_GROUP         = ZR_FLAG(14),
    /* Marks the window as window widget group */
    ZR_WINDOW_POPUP         = ZR_FLAG(15),
    /* Marks the window as a popup window */
    ZR_WINDOW_NONBLOCK      = ZR_FLAG(16),
    /* Marks the window as a nonblock popup window */
    ZR_WINDOW_CONTEXTUAL    = ZR_FLAG(17),
    /* Marks the window as a combo box or menu */
    ZR_WINDOW_COMBO         = ZR_FLAG(18),
    /* Marks the window as a combo box */
    ZR_WINDOW_MENU          = ZR_FLAG(19),
    /* Marks the window as a menu */
    ZR_WINDOW_TOOLTIP       = ZR_FLAG(20),
    /* Marks the window as a menu */
    ZR_WINDOW_REMOVE_ROM    = ZR_FLAG(21)
    /* Removes the read only mode at the end of the window */
};

struct zr_popup_state {
    struct zr_window *win;
    enum zr_window_flags type;
    zr_hash name;
    int active;
    unsigned combo_count;
    unsigned con_count, con_old;
    unsigned active_con;
};

struct zr_edit_state {
    zr_hash name;
    zr_size cursor;
    struct zr_text_selection sel;
    float scrollbar;
    unsigned int seq;
    unsigned int old;
    int active, prev;
};

struct zr_combo_filter_state {
    zr_hash name;
    char filter[ZR_MAX_COMBO_EDIT_BUFFER];
    zr_size length;
    zr_size cursor;
    unsigned int seq;
    unsigned int old;
    int active, prev;
};

struct zr_property_state {
    int active, prev;
    char buffer[ZR_MAX_NUMBER_BUFFER];
    zr_size length;
    zr_size cursor;
    zr_hash name;
    unsigned int seq;
    unsigned int old;
    int state;
};

struct zr_table;
struct zr_window {
    zr_hash name;
    unsigned int seq;
    struct zr_rect bounds;
    zr_flags flags;
    struct zr_scroll scrollbar;
    struct zr_command_buffer buffer;
    struct zr_panel *layout;

    /* persistent widget state */
    struct zr_property_state property;
    struct zr_popup_state popup;
    struct zr_edit_state edit;
    struct zr_combo_filter_state combo;

    struct zr_table *tables;
    unsigned short table_count;
    unsigned short table_size;

    /* window list hooks */
    struct zr_window *next;
    struct zr_window *prev;
    struct zr_window *parent;
};

/*==============================================================
 *                          CONTEXT
 * =============================================================*/
#if ZR_COMPILE_WITH_VERTEX_BUFFER
struct zr_convert_config {
    float global_alpha;
    /* line thickness mininum: 1*/
    enum zr_anti_aliasing line_AA;
    /* line anti-aliasing flag can be turned off if you are thight on memory */
    enum zr_anti_aliasing shape_AA;
    /* shape anti-aliasing flag can be turned off if you are thight on memory */
    unsigned int circle_segment_count;
    /* number of segments used for circles: default to 22 */
    unsigned int arc_segment_count;
    /* number of segments used for arcs: default to 22 */
    unsigned int curve_segment_count;
    /* number of segments used for curves: default to 22 */
    struct zr_draw_null_texture null;
    /* handle to texture with a white pixel to draw shapes */
};

struct zr_canvas {
    float global_alpha;
    enum zr_anti_aliasing shape_AA;
    enum zr_anti_aliasing line_AA;
    struct zr_draw_null_texture null;
    struct zr_rect clip_rect;
    struct zr_buffer *buffer;
    struct zr_buffer *vertices;
    struct zr_buffer *elements;
    unsigned int element_count;
    unsigned int vertex_count;
    zr_size cmd_offset;
    unsigned int cmd_count;
    unsigned int path_count;
    unsigned int path_offset;
    struct zr_vec2 circle_vtx[12];
#if ZR_COMPILE_WITH_COMMAND_USERDATA
    zr_handle userdata;
#endif
};
#endif

struct zr_context {
/* public: can be accessed freely */
    struct zr_input input;
    struct zr_style style;
    struct zr_buffer memory;
    struct zr_clipboard clip;
    zr_flags last_widget_state;

/* private:
    should only be accessed if you
    know what you are doing */
#if ZR_COMPILE_WITH_VERTEX_BUFFER
    struct zr_canvas canvas;
#endif
#if ZR_COMPILE_WITH_COMMAND_USERDATA
    zr_handle userdata;
#endif

    /* windows */
    int build;
    void *pool;
    struct zr_window *begin;
    struct zr_window *end;
    struct zr_window *active;
    struct zr_window *current;
    struct zr_window *freelist;
    unsigned int count;
    unsigned int seq;
};

/*--------------------------------------------------------------
 *                          CONTEXT
 * -------------------------------------------------------------*/
int zr_init_fixed(struct zr_context*, void *memory, zr_size size,
                    const struct zr_user_font*);
int zr_init_custom(struct zr_context*, struct zr_buffer *cmds,
                    struct zr_buffer *pool, const struct zr_user_font*);
int zr_init(struct zr_context*, struct zr_allocator*,
            const struct zr_user_font*);
void zr_clear(struct zr_context*);
void zr_free(struct zr_context*);
#if ZR_COMPILE_WITH_COMMAND_USERDATA
void zr_set_user_data(struct zr_context*, zr_handle handle);
#endif

/*--------------------------------------------------------------
 *                          WINDOW
 * -------------------------------------------------------------*/
int zr_begin(struct zr_context*, struct zr_panel*, const char *title,
            struct zr_rect bounds, zr_flags flags);
void zr_end(struct zr_context*);

struct zr_window* zr_window_find(struct zr_context *ctx, const char *name);
struct zr_rect zr_window_get_bounds(const struct zr_context*);
struct zr_vec2 zr_window_get_position(const struct zr_context*);
struct zr_vec2 zr_window_get_size(const struct zr_context*);
float zr_window_get_width(const struct zr_context*);
float zr_window_get_height(const struct zr_context*);
struct zr_rect zr_window_get_content_region(struct zr_context*);
struct zr_vec2 zr_window_get_content_region_min(struct zr_context*);
struct zr_vec2 zr_window_get_content_region_max(struct zr_context*);
struct zr_vec2 zr_window_get_content_region_size(struct zr_context*);
struct zr_command_buffer* zr_window_get_canvas(struct zr_context*);
struct zr_panel* zr_window_get_panel(struct zr_context*);
int zr_window_has_focus(const struct zr_context*);
int zr_window_is_hovered(struct zr_context*);
int zr_window_is_any_hovered(struct zr_context*);
int zr_window_is_collapsed(struct zr_context*, const char*);
int zr_window_is_closed(struct zr_context*, const char*);
int zr_window_is_active(struct zr_context*, const char*);

void zr_window_close(struct zr_context *ctx, const char *name);
void zr_window_set_bounds(struct zr_context*, struct zr_rect);
void zr_window_set_position(struct zr_context*, struct zr_vec2);
void zr_window_set_size(struct zr_context*, struct zr_vec2);
void zr_window_set_focus(struct zr_context*, const char *name);
void zr_window_collapse(struct zr_context*, const char *name,
                        enum zr_collapse_states);
void zr_window_collapse_if(struct zr_context*, const char *name,
                            enum zr_collapse_states, int cond);
void zr_window_show(struct zr_context*, const char *name, enum zr_show_states);
void zr_window_show_if(struct zr_context*, const char *name,
                        enum zr_show_states, int cond);

/*--------------------------------------------------------------
 *                          DRAWING
 * -------------------------------------------------------------*/
#define zr_command(t, c) ((const struct zr_command_##t*)c)
#define zr_foreach(c, ctx) for((c)=zr__begin(ctx); (c)!=0; (c)=zr__next(ctx, c))
const struct zr_command* zr__next(struct zr_context*, const struct zr_command*);
const struct zr_command* zr__begin(struct zr_context*);

#if ZR_COMPILE_WITH_VERTEX_BUFFER
void zr_convert(struct zr_context*, struct zr_buffer *cmds,
                struct zr_buffer *vertices, struct zr_buffer *elements,
                const struct zr_convert_config*);

#define zr_draw_foreach(cmd,ctx, b)\
    for((cmd)=zr__draw_begin(ctx, b); (cmd)!=0; (cmd)=zr__draw_next(cmd, b, ctx))
const struct zr_draw_command* zr__draw_begin(const struct zr_context*,
                                            const struct zr_buffer*);
const struct zr_draw_command* zr__draw_next(const struct zr_draw_command*,
                                            const struct zr_buffer*,
                                            const struct zr_context*);
#endif
/*--------------------------------------------------------------
 *                          INPUT
 * -------------------------------------------------------------*/
void zr_input_begin(struct zr_context*);
void zr_input_motion(struct zr_context*, int x, int y);
void zr_input_key(struct zr_context*, enum zr_keys, int down);
void zr_input_button(struct zr_context*, enum zr_buttons, int x, int y, int down);
void zr_input_scroll(struct zr_context*, float y);
void zr_input_glyph(struct zr_context*, const zr_glyph);
void zr_input_char(struct zr_context*, char);
void zr_input_unicode(struct zr_context *in, zr_rune);
void zr_input_end(struct zr_context*);

/*--------------------------------------------------------------
 *                          STYLE
 * -------------------------------------------------------------*/
void zr_style_default(struct zr_context*);
void zr_style_set_font(struct zr_context*, const struct zr_user_font*);

/*--------------------------------------------------------------
 *                          LAYOUT
 * -------------------------------------------------------------*/
/* columns layouting with generated position, width and fixed height */
void zr_layout_row_dynamic(struct zr_context*, float height, int cols);
void zr_layout_row_static(struct zr_context*, float height, int item_width, int cols);

/* custom widget width and fixed height */
void zr_layout_row_begin(struct zr_context*, enum zr_layout_format,
                        float row_height, int cols);
void zr_layout_row_push(struct zr_context*, float value);
void zr_layout_row_end(struct zr_context*);
void zr_layout_row(struct zr_context*, enum zr_layout_format, float height,
                    int cols, const float *ratio);

/* custom position and size of widgets */
void zr_layout_space_begin(struct zr_context*, enum zr_layout_format,
                            float height, int widget_count);
void zr_layout_space_push(struct zr_context*, struct zr_rect);
void zr_layout_space_end(struct zr_context*);

/* utility functions */
struct zr_rect zr_layout_space_bounds(struct zr_context*);
struct zr_vec2 zr_layout_space_to_screen(struct zr_context*, struct zr_vec2);
struct zr_vec2 zr_layout_space_to_local(struct zr_context*, struct zr_vec2);
struct zr_rect zr_layout_space_rect_to_screen(struct zr_context*, struct zr_rect);
struct zr_rect zr_layout_space_rect_to_local(struct zr_context*, struct zr_rect);

/* group layout */
int zr_group_begin(struct zr_context*, struct zr_panel*, const char *title, zr_flags);
void zr_group_end(struct zr_context*);

/* tree layout */
#define zr_layout_push(ctx, type, title, state)\
    zr__layout_push(ctx, type, title, state, __FILE__,__LINE__)
int zr__layout_push(struct zr_context*, enum zr_layout_node_type, const char *title,
                    enum zr_collapse_states initial_state, const char *file, int line);
void zr_layout_pop(struct zr_context*);

/*--------------------------------------------------------------
 *                          WIDGETS
 * -------------------------------------------------------------*/
/* base widget calls for custom widgets */
enum zr_widget_layout_states zr_widget(struct zr_rect*, const struct zr_context*);
enum zr_widget_layout_states zr_widget_fitting(struct zr_rect*, struct zr_context*,
                                                struct zr_vec2 item_padding);
/* utilities (working on the next widget) */
struct zr_rect zr_widget_bounds(struct zr_context*);
struct zr_vec2 zr_widget_position(struct zr_context*);
struct zr_vec2 zr_widget_size(struct zr_context*);
int zr_widget_is_hovered(struct zr_context*);
int zr_widget_is_mouse_clicked(struct zr_context*, enum zr_buttons);
int zr_widget_has_mouse_click_down(struct zr_context*, enum zr_buttons, int down);
void zr_spacing(struct zr_context*, int cols);

/* content output widgets */
void zr_text(struct zr_context*, const char*, zr_size, zr_flags);
void zr_text_colored(struct zr_context*, const char*, zr_size, zr_flags,
                    struct zr_color);
void zr_text_wrap(struct zr_context*, const char*, zr_size);
void zr_text_wrap_colored(struct zr_context*, const char*, zr_size, struct zr_color);
void zr_label(struct zr_context*, const char*, zr_flags);
void zr_label_colored(struct zr_context*, const char*, zr_flags align, struct zr_color);
void zr_label_wrap(struct zr_context*, const char*);
void zr_label_colored_wrap(struct zr_context*, const char*, struct zr_color);
void zr_image(struct zr_context*, struct zr_image);

/* buttons */
int zr_button_text(struct zr_context *ctx, const char *title, zr_size len, enum zr_button_behavior);
int zr_button_label(struct zr_context *ctx, const char *title, enum zr_button_behavior);
int zr_button_color(struct zr_context*, struct zr_color, enum zr_button_behavior);
int zr_button_symbol(struct zr_context*, enum zr_symbol_type, enum zr_button_behavior);
int zr_button_image(struct zr_context*, struct zr_image img, enum zr_button_behavior);
int zr_button_symbol_label(struct zr_context*, enum zr_symbol_type, const char*,
                            zr_flags text_alignment, enum zr_button_behavior);
int zr_button_symbol_text(struct zr_context*, enum zr_symbol_type, const char*,
                            zr_size, zr_flags alignment, enum zr_button_behavior);
int zr_button_image_label(struct zr_context*, struct zr_image img, const char*,
                            zr_flags text_alignment, enum zr_button_behavior);
int zr_button_image_text(struct zr_context*, struct zr_image img, const char*,
                            zr_size, zr_flags alignment, enum zr_button_behavior);

/* checkbox */
int zr_check_label(struct zr_context*, const char*, int active);
int zr_check_text(struct zr_context*, const char*, zr_size,int active);
int zr_checkbox_label(struct zr_context*, const char*, int *active);
int zr_checkbox_text(struct zr_context*, const char*, zr_size, int *active);

/* radio button */
int zr_radio_label(struct zr_context*, const char*, int *active);
int zr_radio_text(struct zr_context*, const char*, zr_size, int *active);
int zr_option_label(struct zr_context*, const char*, int active);
int zr_option_text(struct zr_context*, const char*, zr_size, int active);

/* selectable */
int zr_selectable_label(struct zr_context*, const char*, zr_flags align, int *value);
int zr_selectable_text(struct zr_context*, const char*, zr_size, zr_flags align, int *value);
int zr_select_label(struct zr_context*, const char*, zr_flags align, int value);
int zr_select_text(struct zr_context*, const char*, zr_size, zr_flags align, int value);

/* slider */
float zr_slide_float(struct zr_context*, float min, float val, float max, float step);
int zr_slide_int(struct zr_context*, int min, int val, int max, int step);
int zr_slider_float(struct zr_context*, float min, float *val, float max, float step);
int zr_slider_int(struct zr_context*, int min, int *val, int max, int step);

/* progressbar */
int zr_progress(struct zr_context*, zr_size *cur, zr_size max, int modifyable);
zr_size zr_prog(struct zr_context*, zr_size cur, zr_size max, int modifyable);

/* color picker */
struct zr_color zr_color_picker(struct zr_context*, struct zr_color,
                                enum zr_color_picker_format);
int zr_color_pick(struct zr_context*, struct zr_color*,
                    enum zr_color_picker_format);

/* property (dragging, increment/decrement and text input) */
void zr_property_float(struct zr_context *layout, const char *name,
                        float min, float *val, float max, float step,
                        float inc_per_pixel);
void zr_property_int(struct zr_context *layout, const char *name,
                    int min, int *val, int max, int step, int inc_per_pixel);
float zr_propertyf(struct zr_context *layout, const char *name, float min,
                    float val, float max, float step, float inc_per_pixel);
int zr_propertyi(struct zr_context *layout, const char *name, int min, int val,
                int max, int step, int inc_per_pixel);

/* text manipulation */
zr_flags zr_edit_string(struct zr_context*, zr_flags, char *buffer, zr_size *len,
                        zr_size max, zr_filter);
zr_flags zr_edit_buffer(struct zr_context*, zr_flags, struct zr_buffer*, zr_filter);

/* simple chart */
int zr_chart_begin(struct zr_context*, enum zr_chart_type, int num, float min, float max);
zr_flags zr_chart_push(struct zr_context*, float);
void zr_chart_end(struct zr_context*);
void zr_plot(struct zr_context*, enum zr_chart_type, const float *values,
            int count, int offset);
void zr_plot_function(struct zr_context*, enum zr_chart_type, void *userdata,
                        float(*value_getter)(void* user, int index),
                        int count, int offset);

/*--------------------------------------------------------------
 *                          POPUPS
 * -------------------------------------------------------------*/
/* normal blocking popups */
int zr_popup_begin(struct zr_context*, struct zr_panel*, enum zr_popup_type,
                    const char*, zr_flags, struct zr_rect bounds);
void zr_popup_close(struct zr_context*);
void zr_popup_end(struct zr_context*);

/* abstract combo box */
int zr_combo_begin_text(struct zr_context*, struct zr_panel*,
                        const char *selected, zr_size, int max_height);
int zr_combo_begin_label(struct zr_context*, struct zr_panel*,
                        const char *selected, int max_height);
int zr_combo_begin_color(struct zr_context*, struct zr_panel*,
                        struct zr_color color, int max_height);
int zr_combo_begin_symbol(struct zr_context*, struct zr_panel*,
                        enum zr_symbol_type,  int max_height);
int zr_combo_begin_symbol_label(struct zr_context*, struct zr_panel*,
                            const char *selected, enum zr_symbol_type, int height);
int zr_combo_begin_symbol_text(struct zr_context*, struct zr_panel*,
                            const char *selected, zr_size, enum zr_symbol_type, int height);
int zr_combo_begin_image(struct zr_context*, struct zr_panel*,
                        struct zr_image img,  int max_height);
int zr_combo_begin_image_label(struct zr_context*, struct zr_panel*,
                            const char *selected, struct zr_image, int height);
int zr_combo_begin_image_text(struct zr_context*, struct zr_panel*,
                            const char *selected, zr_size, struct zr_image, int height);
int zr_combo_item_label(struct zr_context*, const char*, zr_flags alignment);
int zr_combo_item_text(struct zr_context*, const char*,zr_size, zr_flags alignment);
int zr_combo_item_image_label(struct zr_context*, struct zr_image, const char*,
                            zr_flags alignment);
int zr_combo_item_image_text(struct zr_context*, struct zr_image, const char*,
                            zr_size,zr_flags alignment);
int zr_combo_item_symbol_label(struct zr_context*, enum zr_symbol_type,
                                const char*, zr_flags alignment);
int zr_combo_item_symbol_text(struct zr_context*, enum zr_symbol_type,
                                const char*, zr_size, zr_flags alignment);
void zr_combo_close(struct zr_context*);
void zr_combo_end(struct zr_context*);

/* combobox */
int zr_combo(struct zr_context*, const char **items, int count, int selected,
            int item_height);
int zr_combo_seperator(struct zr_context*, const char *items_seperated_by_seperator,
                    int seperator, int selected, int count, int item_height);
int zr_combo_string(struct zr_context*, const char *items_seperated_by_zeros,
                    int selected, int count, int item_height);
int zr_combo_callback(struct zr_context*,
                        void(item_getter)(void* data, int id, const char **out_text),
                        void *userdata, int selected, int count, int item_height);

void zr_combobox(struct zr_context*, const char **items, int count, int *selected,
                int item_height);
void zr_combobox_string(struct zr_context*, const char *items_seperated_by_zeros,
                        int *selected, int count, int item_height);
void zr_combobox_seperator(struct zr_context*, const char *items_seperated_by_seperator,
                        int seperator,int *selected, int count, int item_height);
void zr_combobox_callback(struct zr_context*,
                        void(item_getter)(void* data, int id, const char **out_text),
                        void *userdata, int *selected, int count, int item_height);

/* contextual */
int zr_contextual_begin(struct zr_context*, struct zr_panel*, zr_flags,
                        struct zr_vec2, struct zr_rect trigger_bounds);
int zr_contextual_item_text(struct zr_context*, const char*, zr_size,zr_flags align);
int zr_contextual_item_label(struct zr_context*, const char*, zr_flags align);
int zr_contextual_item_image_label(struct zr_context*, struct zr_image,
                                    const char*, zr_flags alignment);
int zr_contextual_item_image_text(struct zr_context*, struct zr_image,
                                const char*, zr_size len, zr_flags alignment);
int zr_contextual_item_symbol_label(struct zr_context*, enum zr_symbol_type,
                                    const char*, zr_flags alignment);
int zr_contextual_item_symbol_text(struct zr_context*, enum zr_symbol_type,
                                    const char*, zr_size, zr_flags alignment);
void zr_contextual_close(struct zr_context*);
void zr_contextual_end(struct zr_context*);

/* tooltip */
void zr_tooltip(struct zr_context*, const char*);
int zr_tooltip_begin(struct zr_context*, struct zr_panel*, float width);
void zr_tooltip_end(struct zr_context*);

/*--------------------------------------------------------------
 *                          MENU
 * -------------------------------------------------------------*/
void zr_menubar_begin(struct zr_context*);
void zr_menubar_end(struct zr_context*);

int zr_menu_begin_text(struct zr_context*, struct zr_panel*, const char*,
                        zr_size, zr_flags align, float width);
int zr_menu_begin_label(struct zr_context*, struct zr_panel*, const char*,
                        zr_flags align, float width);
int zr_menu_begin_image(struct zr_context*, struct zr_panel*, const char*,
                        struct zr_image, float width);
int zr_menu_begin_image_text(struct zr_context*, struct zr_panel*, const char*,
                            zr_size,zr_flags align,struct zr_image, float width);
int zr_menu_begin_image_label(struct zr_context*, struct zr_panel*, const char*,
                            zr_flags align,struct zr_image, float width);
int zr_menu_begin_symbol(struct zr_context*, struct zr_panel*, const char*,
                        enum zr_symbol_type, float width);
int zr_menu_begin_symbol_text(struct zr_context*, struct zr_panel*, const char*,
                            zr_size,zr_flags align,enum zr_symbol_type, float width);
int zr_menu_begin_symbol_label(struct zr_context*, struct zr_panel*, const char*,
                            zr_flags align,enum zr_symbol_type, float width);
int zr_menu_item_text(struct zr_context*, const char*, zr_size,zr_flags align);
int zr_menu_item_label(struct zr_context*, const char*, zr_flags alignment);
int zr_menu_item_image_label(struct zr_context*, struct zr_image,
                            const char*, zr_flags alignment);
int zr_menu_item_image_text(struct zr_context*, struct zr_image,
                            const char*, zr_size len, zr_flags alignment);
int zr_menu_item_symbol_text(struct zr_context*, enum zr_symbol_type,
                            const char*, zr_size, zr_flags alignment);
int zr_menu_item_symbol_label(struct zr_context*, enum zr_symbol_type,
                            const char*, zr_flags alignment);
void zr_menu_close(struct zr_context*);
void zr_menu_end(struct zr_context*);

#ifdef __cplusplus
}
#endif

#endif /* ZR_H_ */
