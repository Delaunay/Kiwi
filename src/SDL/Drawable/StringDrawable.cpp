#include "../StyleManager.h"
#include "StringDrawable.h"

namespace kiwi {
	
	i32 StringDrawable::glyph_width() const {
        log_trace();
        return StyleManager::style_manager().width();
    }

    i32 StringDrawable::glyph_height() const {
        log_trace();
        return StyleManager::style_manager().height();
    }
}