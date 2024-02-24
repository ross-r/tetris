#include <game/game.hpp>

#include <ext/imgui/imgui.h>

#include <application.hpp>
#include <window.hpp>

#undef min
#undef max

game::Game::Game() : m_board( this ) {
  m_draw_metrics = true;
  m_paused = false;

  m_music = app::Audio( TEXT( "Tetris.wav" ) );
  m_music.set_volume( 0.05F );
  m_music.play( true );
}

void game::Game::update( const app::Application& app, const double t, const double dt ) {
  if( m_paused ) {
    if( m_board.is_game_over() ) {
      m_paused = false;
    }

    return;
  }

  m_board.physics( t, dt );
  m_board.update();
}

void game::Game::draw( const app::Application& app, const app::Window& window ) {
  if( ImGui::IsKeyPressed( ImGuiKey_P ) ) {
    m_paused = !m_paused;
  }

  ImDrawList* draw_list = ImGui::GetForegroundDrawList();
  ImFont* font = ImGui::GetFont();

  const float window_center_x = ( window.width() / 2 );
  const float window_center_y = ( window.height() / 2 );

  // Draw the board in the center of the window viewport.
  m_board.draw(
    window_center_x - ( m_board.width() / 2 ),
    window_center_y - ( m_board.height() / 2 ) );

  if( m_paused ) {
    const char* paused_str = "GAME PAUSED";
    const auto& paused_text_size = font->CalcTextSizeA( 32.F, 9999.F, 9999.F, paused_str );

    draw_list->AddRectFilled( { 0.F, 0.F }, { ( float ) window.width(), ( float ) window.height() }, 0x7F000000 );
    draw_list->AddText( { window_center_x - ( paused_text_size.x / 2.F ), window_center_y - ( paused_text_size.y / 2.F ) }, 0xFFFFFFFF, paused_str );
  }

  if( m_board.is_game_over() ) {
    const char* paused_str = "GAME OVER";
    const auto& paused_text_size = font->CalcTextSizeA( 32.F, 9999.F, 9999.F, paused_str );

    draw_list->AddRectFilled( { 0.F, 0.F }, { ( float ) window.width(), ( float ) window.height() }, 0x7F000000 );
    draw_list->AddText( { window_center_x - ( paused_text_size.x / 2.F ), window_center_y - ( paused_text_size.y / 2.F ) }, 0xFFFFFFFF, paused_str );
  }

  // Draw controls
  if( 1 ) {
    const char* controls_str = "LEFT ARROW: Move Left\nRIGHT ARROW: Move Right\nR: Rotate\nS: Speed Up\nP: Pause";
    draw_list->AddText( { 16.F, 96.F }, 0xFFFFFFFF, controls_str );
  }

  if( m_draw_metrics ) {
    char buf[ 256 ] = { '\0' };
    sprintf_s( buf, "FPS: %.0F (%.8F)", app.frames_per_second(), app.delta_time() );
    draw_list->AddText( { 2.F, 2.F }, 0xFFFFFFFF, buf );
  }
}
