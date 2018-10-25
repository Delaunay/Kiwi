#pragma once

#include <SFML/Window.hpp>

#undef NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

struct nk_context*   nk_sfml_init(sf::Window* window);

void                 nk_sfml_font_stash_begin(struct nk_font_atlas** atlas);

void                 nk_sfml_font_stash_end();

 int                 nk_sfml_handle_event(sf::Event* event);

void                 nk_sfml_render(enum nk_anti_aliasing, int max_vertex_buffer, int max_element_buffer);

void                 nk_sfml_shutdown();

void                 nk_sfml_device_create();

void                 nk_sfml_device_destroy();
