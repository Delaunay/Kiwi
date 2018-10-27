#ifndef KIWI_CONFIGURATION_HEADER
#define KIWI_CONFIGURATION_HEADER

#include "Point.h"

namespace kiwi {

struct BackEndConfig {
  public:
    static BackEndConfig &config() {
        static BackEndConfig conf;
        return conf;
    }

    bool show_expression_bound_box   = false;
    Color expression_bound_box_color = {250, 250, 150, 0};

    bool show_string_bound_box   = false;
    Color string_bound_box_color = {200, 250, 150, 0};

  private:
    BackEndConfig() = default;
};

} // namespace kiwi

#endif
