#pragma once

#include <cstdint>
#include <memory>

#include <game/shape.hpp>

namespace game {

  enum BlockState {
    state_empty = 0,
  };

  class Board {
  private:
    int m_columns;
    int m_rows;

    // 1D reprensation of a 2D grid board.
    std::unique_ptr< int[] > m_state;

    // All available tetromino to be used for placing.
    Tetromino m_tetromino[ NUM_TETROMINO ] = {
      IShape(),
      OShape(),
      TShape(),
      JShape(),
      LShape(),
      SShape(),
      ZShape()
    };

    uint32_t m_colours[ NUM_TETROMINO ] = {
      0xFF00FFFF,
      0xFFFFFF00,
      0xFFFF00FF,
      0xFF0000FF,
      0xFFFF8100,
      0xFF00FF00,
      0xFFFF0000
    };

    //
    // Tetromino data.
    //
    int m_curr_tetromino_idx;
    int m_next_tetromino_idx;
    Tetromino* m_curr_tetromino;

    //
    // Position data.
    //
    int m_previous_position_x;
    int m_previous_position_y;

    int m_current_position_x;
    int m_current_position_y;

    //
    // Physics data.
    //
    bool m_first_move;
    double m_next_move_time;
    double m_last_move_time;
    double m_next_rotate_time;
    double m_last_rotate_time;
    double m_time_on_line;

    //
    // Game state.
    //
    bool m_game_over;
    int m_level;
    int m_lines_cleared;
    int m_score;

  private:
    // Get the array index for a given row, column combination, access with m_state[...]
    const int get_index( const int row, const int column ) const;

    // Spawn a new tetromino, reset any previous states, etc..
    void new_tetromino();
    bool can_spawn_tetromino();

    // TODO:
    //    Maybe all this can be abstracted out to it's own class.
    //    Piece::move() etc..
    bool can_move_down( const Tetromino& tetromino, const int x, const int y );
    bool can_move_side( const int side /* -1, 1 */ );
    bool can_rotate();

    void rotate_tetromino();

    // Clears the current tetromino's mask from the board.
    void clear_tetromino();

    // Clears the current tetromino's previous mask from the board.
    //    * not to be confused with ""previous tetromino""
    //      this function uses the previous mask for the current tetromino, i.e., previous rotation.
    void clear_prev_tetromino();

    // Draws the tetromino to the board.
    void draw_tetromino();

    //
    // State
    //
    void initialize();

    //
    // Line clearing functions.
    //
    int num_lines_completed();
    bool is_line_complete( const int line );
    void move_line_down( const int line );
    void clear_completed_lines();

    //
    // Score
    //
    void update_score( const int num_lines_completed );

    //
    // Physics
    //
    void physics_start();
    bool physics_rotate( const double t, const double dt );
    void physics_move( const double t, const double dt );
    bool physics_gravity( const double dt );

    //
    // UI
    //
    void draw_preview( const float x, const float y );
    void draw_next_tetromino( const float x, const float y );

  public:
    Board();

  public:
    const int get_state( const int row, const int column ) const;

    void set_state( const int row, const int column, int state );

    const int width() const;
    const int height() const;

  public:
    void draw( const float x, const float y );

    void physics( const double t, const double dt );

    void update();

    void reset();

    const int score() const {
      return m_score;
    }

    const bool is_game_over() const {
      return m_game_over;
    }
  };

}