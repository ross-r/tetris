#pragma once

#include <windows.h>
#include <game/board.hpp>

// forward delcarations.
namespace app {
  class Application;
  class Window;
}

namespace game {

  class Game {
  private:
    Board m_board;

    bool m_draw_metrics;
    bool m_paused;

  public:
    Game();

    void update( const app::Application& app, const double t, const double dt );

    void draw( const app::Application& app, const app::Window& window );

  public:
    Board& board() {
      return m_board;
    }
  };

}