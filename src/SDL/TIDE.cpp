#define SDL_MAIN_HANDLED

#include "SDLWindow.h"
#include "SDLWindowManager.h"

//#include "RenderEngine.h"

#include "../AST/Root.h"

#include "../Parsing/Buffer.h"
#include "../Parsing/Optional.h"
#include "../Parsing/Parser.h"

#include "../Logging/Log.h"
#include "../Types.h"

#include "Drawable/ExpressionDrawable.h"

//#include "Drawable/RenderNode.h"

using namespace kiwi;
using ASTExpressionPtr = generic::Root<Expression<ASTTrait>>;
// using RenderExpressionPtr = generic::Root<Expression<RenderTrait>>;

#include "Drawable/EditableString.h"
#include "Drawable/StringDrawable.h"

#define log_trace(...) LOG_INTERNAL(LogLevel::trace, __VA_ARGS__)

class MyDrawableExpression : public kiwi::Drawable {
  public:
    MyDrawableExpression() {
        log_debug("Initalizing Parser");
        Option<FileBuffer> file =
            FileBuffer::open_buffer("M:\\projects\\Kiwi\\examples\\Testutf8.kw");

        if(file.is_empty()) {
            log_error("File not found");
        } else {
            Parser parser(file.get());
            log_debug("Parsing");

            // --------------------------------------------------------------------
            auto result = parser.parse_declaration(0);
            log_debug("Declaration `", std::get<0>(result), "` was parsed!");
            ASTExpressionPtr expr = std::get<1>(result);
            print_expr<ASTTrait>(std::cout, expr) << std::endl;

            /*/ --------------------------------------------------------------------
            auto class_parsed = parser.parse_declaration(0);
            log_debug("Declaration `", std::get<0>(class_parsed), "` was parsed!");
            ASTExpressionPtr class_expr = std::get<1>(class_parsed);
            print_expr<ASTTrait>(std::cout, class_expr) << std::endl;
            // -------------------------------------------------------------------- */
            // render_expr = convert(expr);

            render_expr = ExpressionDrawable::make(expr.get());
        }
    }

    Point draw(SDL_Renderer *renderer, Point position) override {
        if(!_drawable_cache) {

            auto target = SDL_GetRenderTarget(renderer);
            // Create dummy Offscreen rendering target
            _drawable_cache = TexturePtr{SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                           SDL_TEXTUREACCESS_TARGET, 1, 1)};
            SDL_SetRenderTarget(renderer, _drawable_cache.get());

            // Compute Box size
            // Point4f box = ExpressionRenderEngine::run(renderer, render_expr.get(), { 0, 0 });
            //_size = { box.width, box.height };

            _size = render_expr.get()->size();

            // Create Real target
            _drawable_cache = TexturePtr{SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _size.x, _size.y)};
            SDL_SetRenderTarget(renderer, _drawable_cache.get());

            render_expr.get()->draw(renderer, {0, 0});

            // if (render_expr) {
            // ExpressionRenderEngine::run(renderer, render_expr.get(), { 0, 0 });
            //}
            SDL_SetRenderTarget(renderer, target);
        }

        // std::cout << 0 << " " << 0 << " " << _size.x << " " << _size.y << std::endl;

        SDL_Rect s{0, 0, _size.x, _size.y};
        SDL_Rect t{position.x, position.y, _size.x, _size.y};
        SDL_RenderCopy(renderer, _drawable_cache.get(), &s, &t);

        return {position.x + _size.x, position.y};
    }

    Point size() const override { return _size; }

    void handle_mouse_motion(SDL_MouseMotionEvent event) override {}

    void handle_mouse_wheel(SDL_MouseWheelEvent event) override {}

    void handle_text_editing(SDL_TextEditingEvent event) override {}

    void handle_text_input(SDL_TextInputEvent event) override {}

    void handle_window(SDL_WindowEvent wevent) override {}

    void handle_mouse_button(bool up, SDL_MouseButtonEvent event) override {
        log_info("handle_mouse_button: ", up);
    }

    void handle_keyboard(bool up, SDL_KeyboardEvent event) override {
        log_info("handle_keyboard: ", up);
    }

  private:
    mutable Point _size;
    mutable TexturePtr _drawable_cache;

    std::unique_ptr<ExpressionDrawable> render_expr;
    // RenderExpressionPtr render_expr;
    // RenderExpressionPtr render_class_expr;
};

class MyWindow : public SDLWindow {
  private:
    friend class WindowManager;

    MyWindow(SDL_Window *handle) : SDLWindow(handle) {}

    Array<Drawable *> _entities;

  public:
    /*
    template<typename T, typename ...Args>
    void insert_entity(Args&&... args) {
        _entities.emplace_back<T>(std::forward(args)...);
    }*/
    void insert_entity(Drawable *d) { _entities.push_back(d); }

    // kiwi::ExpressionRenderEngine::ExpressionPtr sqr_node =
    // kiwi::ExpressionRenderEngine::make_sqr();

    void render() override {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(_renderer);
        SDL_SetRenderDrawColor(_renderer, 255, 25, 255, SDL_ALPHA_OPAQUE);

        // log_info("Cleaning Screen");

        draw(_renderer);
        SDL_RenderPresent(_renderer);
    }

    void draw(SDL_Renderer *renderer) override {
        Point next = {50, 50};
        for(auto &drawable : _entities) {
            drawable->set_position(next);
            next = drawable->draw(renderer, next);

            next.x = 50;
            next.y += drawable->size().y + 1;
        }
    }

  private:
    Drawable *_selected = nullptr;

  public:
    void handle_mouse_motion(SDL_MouseMotionEvent event) override {}

    void handle_mouse_wheel(SDL_MouseWheelEvent event) override {}

    void handle_text_editing(SDL_TextEditingEvent event) override {}

    void handle_text_input(SDL_TextInputEvent event) override {
        if(_selected)
            _selected->handle_text_input(event);
    }

    void handle_window(SDL_WindowEvent wevent) override {}

    void handle_mouse_button(bool up, SDL_MouseButtonEvent event) override {
        for(auto &drawable : _entities) {
            Point position = drawable->position();
            Point size     = drawable->size();

            if(drawable->collide(event.x, event.y)) {
                _selected = drawable;
                return drawable->handle_mouse_button(up, event);
            }
        }
        _selected = nullptr;
    }

    void handle_keyboard(bool down, SDL_KeyboardEvent event) override {
        if(_selected)
            _selected->handle_keyboard(down, event);
    }
};

//*
int main() {

    /* * /
    std::string str = u8"abécd";

    for (int i = 0; i < str.length(); ++i) {
        std::cout << get_byte_pos(str, i) << "/" << str.length() << std::endl;
    }*/

    kiwi::Log::instance().set_log_level(kiwi::LogLevel::all);

    log_debug("Trying to create a window");

    kiwi::SDLWindow *w1 = kiwi::WindowManager::new_window<MyWindow>(
        u8"TIDE", 800, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // new StringDrawable(u8"Приве́т");
    MyWindow* mw = reinterpret_cast<MyWindow*>(w1);

    //mw->insert_entity<MyDrawableExpression>();

    log_trace();
    mw->insert_entity(new StringDrawable(u8"Приве́т"));

    log_trace();
    mw->insert_entity(new EditableString(u8"Hellow"));

    log_trace();
    mw->insert_entity(new EditableString(u8"Hellow"));

    log_trace();
    mw->insert_entity(new MyDrawableExpression());

    log_debug("Launching event loop");
    kiwi::WindowManager::manager().run();

    log_debug("Done!");
    return 0;
}
