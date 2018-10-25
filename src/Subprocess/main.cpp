//#include <boost/process.hpp>
//#include <boost/asio.hpp>
#include <thread>
#include <sstream>

#include "Drawable/BuiltinDrawable.h"

/*
#if __linux__

#else
#include <direct.h>
#define GetCurrentDir _getcwd

template<std::size_t size = 2048>
std::string get_working_directory() {
    char path[size];

    for (int i = 0; i < size; ++i)
        path[i] = '\0';

    if (_getcwd(path, size)) {
        return "";
    }

    return std::string(path);
}
#endif*/

//using namespace boost::process;


#define MONGO_DB_LOC "C:\\\"Program Files\"\\MongoDB\\Server\\3.6\\bin\\mongod.exe"
/*
int main() {
    kiwi::Log::instance().set_log_level(kiwi::LogLevel::all);

    log_debug("Initalizing Parser");
    Option<FileBuffer> file = FileBuffer::open_buffer("M:\\projects\\Kiwi\\examples\\Testutf8.kw");
    std::unique_ptr<ExpressionDrawable> render_expr;

    CHECK(SDL_Init(SDL_INIT_VIDEO));
    CHECK(TTF_Init());

    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    if (file.is_empty()) {
        log_error("File not found");
    }
    else {
        Parser parser(file.get());
        log_debug("Parsing");

        // --------------------------------------------------------------------
        auto result = parser.parse_declaration(0);
        log_debug("Declaration `", std::get<0>(result), "` was parsed!");
        ASTExpressionPtr expr = std::get<1>(result);
        print_expr<ASTTrait>(std::cout, expr) << std::endl;


        render_expr = ExpressionDrawable::make(expr.get());
    }

    u32 w = render_expr->size().x;
    u32 h = render_expr->size().y;
    u32 flag = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    WindowPtr handle = WindowPtr{ SDL_CreateWindow("Tile Generation", 20, 20, w * 1, h * 1, flag) };
    RendererPtr renderer{ SDL_CreateRenderer(handle.get(), -1, SDL_RENDERER_ACCELERATED) };

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer.get());
    SDL_SetRenderDrawColor(renderer.get(), 255, 25, 255, SDL_ALPHA_OPAQUE);


    //kiwi::BuiltinDrawable test("Testing");
    //test.draw(renderer.get(), { 0, 0 });


    render_expr->draw(renderer.get(), { 0, 0 });

    SurfacePtr sshot{ SDL_CreateRGBSurface(0, w * 1, h * 1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000) };
    SDL_RenderReadPixels(renderer.get(), NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot.get(), "screenshot.bmp");

    TTF_Quit();
    SDL_Quit();
    return 0;
}*/

