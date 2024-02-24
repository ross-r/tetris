#include <iostream>

#include <window.hpp>
#include <application.hpp>
#include <renderer.hpp>
#include <audio.hpp>

#include <game/game.hpp>
#include <game/board.hpp>
#include <game/shape.hpp>

#include <imgui/imgui_impl_win32.hpp>

app::Application g_app{};
app::Window g_window{};

game::Game g_game{};

bool window_message_handler( UINT message, WPARAM wparam, LPARAM lparam ) {
  if( g_window.imgui_message_handler( message, wparam, lparam ) ) {
    return false;
  }

  // Stop the application if this window gets closed.
  if( message == WM_CLOSE ) {
    g_app.close();
    return true;
  }

  return true;
}

void window_draw( app::Renderer& renderer ) {
  float clear_color[ 4 ] = { 0.1F, 0.1F, 0.1F, 1.F };
  renderer.set_clear_color( clear_color );

  g_game.draw( g_app, g_window );
}

void render( app::Application& app, const double dt ) {
  // Window::draw invokes internal renderer.begin / end between the callback
  // maybe just omit the function all together and manually handle that here.
  g_window.draw( window_draw );
}

void update( app::Application& app, const double t, const double dt ) {
  g_game.update( app, t, dt );
}

int main( int argc, char* argv[] ) {
  printf( "%s\n", argv[ 0 ] );

  // Create the main window.
  g_window = app::Window( TEXT( "TetrisApp001" ), TEXT( "Tetris" ), 1920, 1080 );
  g_window.set_message_handler( window_message_handler );
  g_window.show();
  g_window.center();

  // Start the application and run the main loop routine.
  g_app.exec( render, update );

  // Cleanup.
  app::AudioEngine::get()->shutdown();
  g_window.shutdown();

  return 0;
}