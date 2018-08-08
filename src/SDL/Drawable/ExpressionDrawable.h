#ifndef KIWI_EXPRESSION_DRAWABLE_NODE_HEADER
#define KIWI_EXPRESSION_DRAWABLE_NODE_HEADER

#define NOMINMAX
#include "../StyleManager.h"
#include "../../AST/Expression.h"
#include "../../AST/LightAST.h"
#include "../../Types.h"
#include "../SDLHelper.h"

#include "Drawable.h"
#include "StringDrawable.h"

#include <memory>

namespace kiwi{

class ExpressionDrawable: public Drawable {
public:
    using StringType = StringDrawable;
    using StringView = StringDrawable;

    static std::unique_ptr<ExpressionDrawable> make(Expression<LightExpression>* elem);
    static std::unique_ptr<ExpressionDrawable> make(Type<LightExpression>* elem);

    typedef SDL_Color                SDLColor;
    //typedef std::vector<Type<RenderTrait>*> ArgTypes;
    typedef std::vector<StringType>  ArgNames;
    typedef size_t                   IndexType;

    typedef std::string              StringArgument;

    static std::string to_string(StringType const& str) {
        return str.string();
    }

    template<typename T>
    static UniquePtr<Drawable> make_text(const T& str, const SDLColor& color){
        return std::make_unique<StringType>(str, color, StringType::default_font());
    }

    template<typename T>
    static UniquePtr<Drawable> make_string(const T& str) {
        return make_text(str, { 255, 255, 255 });
    }

    template<typename T>
    static UniquePtr<Drawable> make_type_name(const T& str) {
        return make_text(str, { 255, 200, 200 });
    }

    template<typename T>
    static UniquePtr<Drawable> make_call_name(const T& str) {
        return make_text(str, { 155, 155, 255 });
    }

    template<typename T>
    static UniquePtr<Drawable> make_keyword(const T& str){
        return make_text(str, {255, 155, 155});
    }

    template<typename T>
    static UniquePtr<Drawable> make_placeholder_name(const T& str){
        return make_text(str, {255, 155, 255});
    }

    template<typename T>
    static UniquePtr<Drawable> make_function_name(const T& str){
        return make_text(str, {155, 255, 255});
    }

    template<typename T>
    static UniquePtr<Drawable> make_error_message(const T& str){
        return make_text(str, {255, 155, 155});
    }

    template<typename T>
    static UniquePtr<Drawable> make_builtin_name(const T& str){
        return make_text(str, {155, 255, 155});
    }

    template<typename T>
    static UniquePtr<Drawable> make_argument_name(const T& str){
        return make_text(str, {155, 155, 155});
    }

	Point4f bound_box {10000, 10000, -10000, -10000};


    // Drawable Methods:
    // Event Handling
    virtual void handle_mouse_motion(SDL_MouseMotionEvent event) {}

    virtual void handle_mouse_wheel(SDL_MouseWheelEvent event) {}

    virtual void handle_text_editing(SDL_TextEditingEvent event) {}

    virtual void handle_text_input(SDL_TextInputEvent event) {
        if (_selected) _selected->handle_text_input(event); 
    }

    virtual void handle_window(SDL_WindowEvent wevent) {}

    virtual void handle_mouse_button(bool up, SDL_MouseButtonEvent event) {
        log_info("Node was clicked");
        for (int i = 0; i < _entities.size(); ++i) {
            Drawable* drawable = _entities[i].get();
            Point position = drawable->position();
            Point size = drawable->size();

            if (drawable->collide(event.x, event.y)) {
                _selected = drawable;
                return drawable->handle_mouse_button(up, event);
            }
        }
        _selected = this;
    }

    virtual void handle_keyboard(bool down, SDL_KeyboardEvent event) {
        if (_selected) _selected->handle_keyboard(down, event);
    }

    Point size() const override { return _size;  }
    void set_size(Point size)  { _size = size;  }

    Drawable* insert_entity(UniquePtr<Drawable>&& d) {
        _entities.push_back(std::move(d)); 
        return _entities[_entities.size() - 1].get();
    }

    Tuple<Drawable*, Point> insert_entity_after(UniquePtr<Drawable>&& d, Point pos){
        log_debug("Inserting Entity (name: ", d->drawable_name(), ")");
        Drawable* val = insert_entity(std::forward<UniquePtr<Drawable>>(d));
        Point npos = {pos.x + val->size().x, pos.y};

        log_debug("Old size (x: ", size().x, ") (y: ", size().y, ")");
        set_size({ std::max(npos.x, std::max(pos.x, size().x)), size().y });
        log_debug("New size (x: ", size().x, ") (y: ", size().y, ")");

        log_debug("Old Pos (x: ", pos.x, ") (y: ", pos.y, ")");
        log_debug("New Pos (x: ", npos.x, ") (y: ", npos.y, ")");
        return std::make_tuple(val, npos);
    }

    class NewlineDrawable : public Drawable {
    public:
        NewlineDrawable(Point start_pos = { 0, 0 }) :
            start_pos(start_pos)
        {}

        Point size() const { return { style.height(), style.width() }; }

        Point draw(SDL_Renderer *renderer, Point pos) {
            pos.y += style.height();
            pos.x = start_pos.x;
            return pos;
        }

        String const drawable_name() override {
            return "NewlineDrawable";
        }

    private:
        Point start_pos;
        StyleManager const& style = StyleManager::style_manager();
    };

    #define indent_size 4
    class IndentDrawable : public Drawable {
    public:
        IndentDrawable(Point start_pos = { 0, 0 }, u32 indent = 0) :
            start_pos(start_pos), indent(indent)
        {}

        Point size() const { return { style.height(), style.width() }; }

        Point draw(SDL_Renderer *renderer, Point pos) {
            return Point2f(start_pos.x + style.width() * indent_size * indent, pos.y);
        }

        String const drawable_name() override {
            return "IndentDrawable";
        }

    private:
        u32   indent;
        Point start_pos;
        StyleManager const& style = StyleManager::style_manager();
    };

    Point2f new_line(Point2f pos, Point2f original_pos = {0, 0}) {
        pos.y += style.height();

        set_size({ size().x, size().y + style.height() });

        pos.x = original_pos.x;

        insert_entity(std::make_unique<NewlineDrawable>(original_pos));
        
        return pos;
    }

    Point2f indent(Point2f pos, uint32 idt = 1, Point2f original_pos = { 0, 0 }) {
        std::tie(std::ignore, pos) = insert_entity_after(std::make_unique<IndentDrawable>(original_pos, idt), pos);
        return pos;
    }

    Point2f space(Point2f pos) {
        return Point2f(pos.x + style.width() * 1, pos.y);
    }

    StyleManager const& style = StyleManager::style_manager();

private:
    u32   _line = 1;
    Point _size = { style.width(), style.height() };
    Array<UniquePtr<Drawable>> _entities;
    Drawable* _selected = this;
    mutable TexturePtr _texture = nullptr;

public:
    Point recompute_size(){
        Point size = {style.width(), style.height()};

        for (int i = 0; i < _entities.size(); ++i) {
            Drawable* entity = _entities[i].get();
            size = {
                std::max(size.x, entity->size().x), 
                std::max(size.y, entity->size().y)
            };
        }

        return size;
    }

    String const drawable_name() override {
        return "ExpressionDrawable";
    }
        
    Point off_screen_draw(SDL_Renderer *renderer, Point oposition) {
        static u32 depth = 0;
        depth += 1;

        log_trace();
        if (!_texture) {
            log_cinfo(depth, "Generating Texture (name: ", drawable_name(), ")");
            // Create Offscreen Rendering Target
            _texture = TexturePtr{ CHECK(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _size.x, _size.y)) };
            auto target = SDL_GetRenderTarget(renderer);
            CHECK(SDL_SetRenderTarget(renderer, _texture.get()));
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            Point position = oposition;
            for (int i = 0; i < _entities.size(); ++i) {
                Drawable* entity = _entities[i].get();
                log_cinfo(depth + 1, "Drawing entity (name: ", entity->drawable_name(), ")");
                position = entity->draw(renderer, position);
                log_cinfo(depth + 1, "New pos is (pos: (", position.x, " x ", position.y, "))");
            }

            SDL_SetRenderDrawColor(renderer, 250, 250, 150, 0);
            SDL_RenderDrawRect(renderer, nullptr);
            CHECK(SDL_SetRenderTarget(renderer, target));
        }

        depth -= 1;
        return { oposition.x + _size.x, oposition.y};
    }
    
    Point draw(SDL_Renderer *renderer, Point position) override {
        log_trace();
        log_info("Off Screen Draw");
        // Rendering into its own Sufrace so position is 0, 0
        off_screen_draw(renderer, {0, 0});
        
        SDL_Rect box;
        box.x = i32(position.x);
        box.y = i32(position.y);
        box.w = i32(_size.x);
        box.h = i32(_size.y);
        
        log_info("Copy Cached Texture");
        CHECK(SDL_RenderCopy(renderer, _texture.get(), nullptr, &box));
        return { position.x + _size.x, position.y};
    }
};
}
#endif
