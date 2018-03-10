#pragma once

#include "RenderNode.h"
#include <algorithm>

namespace kiwi{

	// Expression worksapce is a small rectangle in which an expression is rendered to be modified
	class ExpressionWorkspace {
	public:
		typedef RenderNode Node;


	private:
		Node * _selected;

	};

	class Widget {
	public:
		Widget() {}

		virtual void handle_mouse_button_up(SDL_MouseButtonEvent event) {}
		virtual void handle_mouse_button_down(SDL_MouseButtonEvent event) {}
		virtual void handle_keyboard_up(SDL_KeyboardEvent event) {}
		virtual void handle_keyboard_down(SDL_KeyboardEvent event) {}

		virtual void handle_drop_file(SDL_DropEvent event) {}
		virtual void handle_drop_text(SDL_DropEvent event) {}
		virtual void handle_drop_begin(SDL_DropEvent event) {}
		virtual void handle_drop_complete(SDL_DropEvent event) {}

		virtual void handle_mouse_motion(SDL_MouseMotionEvent event) {}
		virtual void handle_mouse_wheel(SDL_MouseWheelEvent event) {}
		virtual void handle_text_editing(SDL_TextEditingEvent event) {}
		virtual void handle_text_input(SDL_TextInputEvent event) {}

		virtual void draw(SDL_Renderer *renderer) {}

		void set_depth(f32 depth) { _depth = depth;  }

		f32   depth()    const { return _depth;     }
		Point size()     const { return _size;      }
		Point position() const { return _position;  }

	protected:
		f32         _depth;
		Point		_position;
		Point		_size;

		SurfacePtr  _surface;
		TexturePtr  _texture;
	};

	class VerticalLayout: public Widget{
	public:
		void add_widget();


		void draw(SDL_Renderer *renderer) {
			std::sort(std::begin(_widgets), std::end(_widgets), [](const Widget& a, const Widget& b) { 
				return a.depth() < b.depth(); 
			});

			Point size;

			for (const Widget& a : _widgets) {
				size = Point{ a.size.x,  size.y < a.size.y ? a.size.y : size.y };
			}

			_surface = SDL_CreateRGBSurface();

		}

	private:
		std::vector<Widget> _widgets;
	};




}