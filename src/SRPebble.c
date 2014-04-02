#include <pebble.h>

static Window *window;

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 4

#define MENU_TITLE_LEN 12


// A simple menu layer can have multiple sections

typedef struct {
	SimpleMenuLayer *menu_layer;
	SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
	SimpleMenuItem menu_items[NUM_FIRST_MENU_ITEMS];
} MenuItemSelectCtx_t;

MenuItemSelectCtx_t menuItemSelectCtx_data;
MenuItemSelectCtx_t *menuItemSelectCtx = &menuItemSelectCtx_data;

static int
menu_items_alloc_title(SimpleMenuItem *smi, const int total) {
	for (int i=0; i < total; ++i) {
		if ((smi[i].title = malloc(MENU_TITLE_LEN)) == NULL) {
			goto malloc_failed;
		}
	}
	return 0;
malloc_failed:
	return -1;
}

static void
menu_items_free_title(SimpleMenuItem *smi, const int total) {
	for (int i=0; i < total; ++i) {
		if (smi[i].title) {
			free((char *)smi[i].title);
			smi[i].title = NULL;
		}
	}
}

static void
menu_item_select_callback(int index, void *ctx_ptr) {
	MenuItemSelectCtx_t *ctx = (MenuItemSelectCtx_t *)ctx_ptr;
	ctx->menu_items[index].subtitle = "Selected";
	layer_mark_dirty(simple_menu_layer_get_layer(ctx->menu_layer));
}


/*
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}
*/

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	SimpleMenuItem *menu_items = menuItemSelectCtx->menu_items;
	SimpleMenuSection *menu_sections = menuItemSelectCtx->menu_sections;

	menu_items_alloc_title(menu_items, NUM_FIRST_MENU_ITEMS);
	for (int i=0; i < NUM_FIRST_MENU_ITEMS; ++i) {
		snprintf((char *)menuItemSelectCtx->menu_items[i].title, MENU_TITLE_LEN, "Item %d", i);
		menu_items[i].callback = menu_item_select_callback;
	}

	menu_sections[0] = (SimpleMenuSection){
		.num_items = NUM_FIRST_MENU_ITEMS,
		.items = menu_items,
	};
  
	menuItemSelectCtx->menu_layer = simple_menu_layer_create(bounds, window, menuItemSelectCtx->menu_sections, NUM_MENU_SECTIONS, menuItemSelectCtx);
	layer_add_child(window_layer, simple_menu_layer_get_layer(menuItemSelectCtx->menu_layer));
}

static void window_unload(Window *window) {
	simple_menu_layer_destroy(menuItemSelectCtx->menu_layer);
	menu_items_free_title(menuItemSelectCtx->menu_items, NUM_FIRST_MENU_ITEMS);
}

int main(void) {
	window = window_create();
	// Setup the window handlers
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	window_stack_push(window, true /* Animated */);

	app_event_loop();
	window_destroy(window);
}
