#include <game/board.hpp>

#include <random>
#include <algorithm>
#include <vector>

#include <ext/imgui/imgui.h>

const float GRID_SIZE = 32.F;
const float GRID_SPACING = 2.F;

const auto& random_number = []( const int min, const int max ) -> int {
  // https://en.cppreference.com/w/cpp/numeric/random
  static std::random_device r;

  std::default_random_engine e1( r() );
  std::uniform_int_distribution<int> uniform_dist( min, max );
  return uniform_dist( e1 );
};

game::Board::Board() {
  m_columns = 20;
  m_rows = 10;
  m_state = std::make_unique< int[] >( m_rows * m_columns );
  reset();
}

const int game::Board::get_index( const int row, const int column ) const {
  if( row < 0 || column < 0 || row >= m_rows || column >= m_columns ) {
    return -1;
  }

  return row * m_columns + column;
}

void game::Board::new_tetromino() {
  m_previous_position_x = 0;
  m_previous_position_y = 0;

  // TODO: better random
  srand( time( NULL ) );
  m_curr_tetromino_idx = m_next_tetromino_idx;
  m_next_tetromino_idx = random_number( 0, NUM_TETROMINO - 1 );

  if( m_curr_tetromino ) {
    m_curr_tetromino->reset();
  }

  m_curr_tetromino = &m_tetromino[ m_curr_tetromino_idx ];

  m_current_position_x = ( m_rows / 2 );
  m_current_position_y = 0;

  // GAME OVER!!
  if( !can_spawn_tetromino() ) {
    m_game_over = true;
  }
}

bool game::Board::can_spawn_tetromino() {
  for( int i{}; i < 4; ++i ) {
    for( int j{}; j < 4; ++j ) {
      const int index = i * 4 + j;

      const int mask = m_curr_tetromino->current_mask();
      if( ( mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      if( get_state( m_current_position_x + i, m_current_position_y + j ) ) {
        return false;
      }
    }
  }

  return true;
}

bool game::Board::can_move_down( const Tetromino& tetromino, const int x, const int y ) {
  const int new_x = x;
  const int new_y = y + 1;

  if( new_y >= m_columns ) {
    return false;
  }

  for( int row{ 0 }; row < 4; ++row ) {
    const int column = tetromino.column_height( row );

    const int index = row * 4 + column;
    const int mask = tetromino.current_mask();
    if( ( mask & ( 1 << index ) ) == 0 ) {
      continue;
    }

    if( ( new_y + column ) >= m_columns ) {
      return false;
    }

    if( get_state( new_x + row, new_y + column ) ) {
      return false;
    }
  }

  return true;
}

bool game::Board::can_move_side( const int side ) {
  const int x = m_current_position_x + side;
  const int y = m_current_position_y;

  for( int column{ 0 }; column < 4; ++column ) {
    const int row = side == 1 ? m_curr_tetromino->row_end( column ) :
      m_curr_tetromino->row_start( column );

    const int index = row * 4 + column;
    const int mask = m_curr_tetromino->current_mask();
    if( ( mask & ( 1 << index ) ) == 0 ) {
      continue;
    }

    // Clamp to board bounds.
    if( ( x + row ) >= m_rows || ( x + row ) < 0 ) {
      return false;
    }

    // If the cell at the given row, column is set, we can't move there.
    if( get_state( x + row, y + column ) ) {
      return false;
    }
  }

  return true;
}

bool game::Board::can_rotate() {
  const int x = m_current_position_x;
  const int y = m_current_position_y;

  for( int row{}; row < 4; ++row ) {
    for( int column{}; column < 4; ++column ) {
      const int index = row * 4 + column;

      const int curr_mask = m_curr_tetromino->current_mask();
      const int next_mask = m_curr_tetromino->next_mask();
      if( ( next_mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      // Skip any overlapping cells.
      if( ( curr_mask & ( 1 << index ) ) && ( next_mask & ( 1 << index ) ) ) {
        continue;
      }

      if( ( x + row ) >= m_rows || x < 0 ) {
        return false;
      }

      if( ( y + column ) >= m_columns || y < 0 ) {
        return false;
      }

      if( get_state( x + row, y + column ) ) {
        return false;
      }
    }
  }

  return true;
}

void game::Board::rotate_tetromino() {
  clear_tetromino();
  m_curr_tetromino->rotate();
}

void game::Board::clear_tetromino() {
  for( int i{}; i < 4; ++i ) {
    for( int j{}; j < 4; ++j ) {
      const int index = i * 4 + j;

      const int mask = m_curr_tetromino->current_mask();
      if( ( mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      set_state( m_current_position_x + i, m_current_position_y + j, 0 );
    }
  }
}

void game::Board::clear_prev_tetromino() {
  for( int i{}; i < 4; ++i ) {
    for( int j{}; j < 4; ++j ) {
      const int index = i * 4 + j;

      const int mask = m_curr_tetromino->current_mask();
      if( ( mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      set_state( m_previous_position_x + i, m_previous_position_y + j, 0 );
    }
  }
}

void game::Board::draw_tetromino() {
  for( int i{}; i < 4; ++i ) {
    for( int j{}; j < 4; ++j ) {
      const int index = i * 4 + j;

      const int mask = m_curr_tetromino->current_mask();
      if( ( mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      set_state( m_current_position_x + i, m_current_position_y + j, 1 + m_curr_tetromino_idx );
    }
  }
}

void game::Board::initialize() {
  //
  // Game state.
  //
  m_game_over = false;
  m_level = 0;
  m_lines_cleared = 0;
  m_score = 0;

  //
  // Physics data.
  //
  m_first_move = true;
  m_next_move_time = 0.0;
  m_last_move_time = 0.0;
  m_next_rotate_time = 0.0;
  m_last_rotate_time = 0.0;
  m_time_on_line = 0.0;

  //
  // Tetromino data.
  //
  m_curr_tetromino = nullptr;
  m_next_tetromino_idx = random_number( 0, NUM_TETROMINO - 1 );
  m_curr_tetromino_idx = m_next_tetromino_idx;
  new_tetromino();
}

void game::Board::reset() {
  //
  // Reset the board state.
  //
  memset( &m_state[ 0 ], 0, sizeof( int ) * m_rows * m_columns );

  //
  // Initialize all board related data (physics, etc..)
  //
  initialize();
}

int game::Board::num_lines_completed() {
  int lines = 0;

  for( int line = m_columns - 1; line >= 0; --line ) {
    // Skip any incomplete lines.
    if( !is_line_complete( line ) ) {
      continue;
    }

    ++lines;
  }

  return lines;
}

bool game::Board::is_line_complete( const int line ) {
  for( int row{}; row < m_rows; ++row ) {
    if( !get_state( row, line ) ) {
      return false;
    }
  }

  return true;
}

void game::Board::move_line_down( const int line ) {
  for( int i = 0; i < m_rows; ++i ) {
    int state = get_state( i, line );
    set_state( i, line + 1, state );
    set_state( i, line, 0 );
  }
}

void game::Board::clear_completed_lines() {
  // Check all lines of the board from the bottom up and clear any completed lines, shifting
  // all the above lines downwards by 1 and retaining their states.

  if( num_lines_completed() == 0 ) {
    return;
  }

  // Update the score for the number of lines completed.
  update_score( num_lines_completed() );

  // Clear all the completed lines.
  do {
    for( int line = m_columns - 1; line >= 0; --line ) {
      // Skip any incomplete lines.
      if( !is_line_complete( line ) ) {
        continue;
      }

      // Clear the line.
      for( int row = 0; row < m_rows; ++row ) {
        set_state( row, line, 0 );
      }

      // No more lines above that can be moved down.
      if( line - 1 == 0 ) {
        break;
      }

      // For all the lines above this line, move them down by 1.
      for( int i = line - 1; i >= 0; --i ) {
        move_line_down( i );
      }
    }
  }
  while( num_lines_completed() > 0 );
}

void game::Board::update_score( const int num_lines_completed ) {
  const int base_scores[] = {
    // single
    40,

    // double
    100,

    // triple
    300,

    // tetris
    1200
  };

  // Update score.
  const int score = base_scores[ num_lines_completed - 1 ] * ( m_level + 1 );
  m_score += score;

  // Update level (we're using A-Type level system)
  m_lines_cleared += num_lines_completed;
  m_level = ceil( ( float ) ( m_lines_cleared / 10 ) );
}

const int game::Board::get_state( const int row, const int column ) const {
  const int index = get_index( row, column );
  if( index == -1 ) {
    return 0;
  }

  return m_state[ index ];
}

void game::Board::set_state( const int row, const int column, int state ) {
  const int index = get_index( row, column );
  if( index == -1 ) {
    return;
  }

  m_state[ index ] = state;
}

const int game::Board::width() const {
  return ( GRID_SIZE + GRID_SPACING ) * m_rows + GRID_SPACING;
}

const int game::Board::height() const {
  return ( GRID_SIZE + GRID_SPACING ) * m_columns + GRID_SPACING;
}

void game::Board::draw( const float x, const float y ) {
  ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

  float current_x = x;
  float current_y = y;

  //
  // Draw the board grid and all tetromino colours.
  //
  for( int row{}; row < m_rows; ++row ) {
    for( int column{}; column < m_columns; ++column ) {
      if( get_state( row, column ) > 0 ) {
        const int tetromino_idx = get_state( row, column ) - 1;
        const uint32_t col = m_colours[ tetromino_idx ];
        draw_list->AddRectFilled( { current_x, current_y }, { current_x + GRID_SIZE, current_y + GRID_SIZE }, col, 4.F );
      }
      else {
        draw_list->AddRect( 
          { current_x, current_y }, 
          { current_x + GRID_SIZE, current_y + GRID_SIZE }, 
          0x3FFFFFFF, 
          4.F 
        );
      }

      current_y += GRID_SIZE + GRID_SPACING;
    }

    current_x += GRID_SIZE + GRID_SPACING;
    current_y = y;
  }

  draw_list->AddRect(
    { x - ( GRID_SPACING * 2.F ), y - ( GRID_SPACING * 2.F ) },
    {
      x + ( GRID_SIZE + GRID_SPACING ) * m_rows + GRID_SPACING,
      y + ( GRID_SIZE + GRID_SPACING ) * m_columns + GRID_SPACING
    },
    0x7FFFFFFF
  );

  //
  // Draw game information.
  //
  //{
  //  char buf[ 256 ] = { '\0' };
  //  sprintf_s( buf, "MODE: A-TYPE\nSCORE: %d\nLEVEL: %d\nLINES: %d", m_score, m_level + 1, m_lines_cleared );
  //  draw_list->AddText( { ( float ) current_x + 16, current_y }, 0xFFFFFFFF, buf );
  //}

  draw_preview( x, y );
  draw_next_tetromino( x, y );
}

void game::Board::draw_preview( const float x, const float y ) {
  ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

  int position_x = m_current_position_x;
  int position_y = m_current_position_y;

  const Tetromino tetromino{ m_tetromino[ m_curr_tetromino_idx ] };
  while( can_move_down( tetromino, position_x, position_y ) ) {
    position_y += 1;
  }

  const float start_x = x + ( ( GRID_SIZE + GRID_SPACING ) * position_x );
  const float start_y = y + ( ( GRID_SIZE + GRID_SPACING ) * position_y );

  float current_x = start_x;
  float current_y = start_y;

  const uint32_t col = m_colours[ m_curr_tetromino_idx ];
  //const uint32_t col1 = 0x7F000000 | ( col & 0xFFFFFF );

  for( int i{}; i < 4; ++i ) {
    for( int j{}; j < 4; ++j ) {
      const int index = i * 4 + j;

      const int mask = m_curr_tetromino->current_mask();
      if( ( mask & ( 1 << index ) ) ) {

        draw_list->AddRect(
          { current_x, current_y },
          { current_x + GRID_SIZE, current_y + GRID_SIZE },
          col,
          4.F,
          0,
          2.F
        );
      }

      current_y += GRID_SIZE + GRID_SPACING;
    }

    current_x += GRID_SIZE + GRID_SPACING;
    current_y = start_y;
  }
}

void game::Board::draw_next_tetromino( const float x, const float y ) {
  ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

  // Create a copy of the next tetromino and reset it to prevent copying over
  // current mask stuff which indicates rotation, normally, this information
  // is reset when a new tetromino is spawned so it's handled that way
  // in the game, but in the preview, we just do a copy of the object.
  Tetromino tetromino{ m_tetromino[ m_next_tetromino_idx ] };
  tetromino.reset();

  const int rows = tetromino.width();
  const int columns = tetromino.height();

  const float start_x = x + width() + GRID_SIZE;
  const float start_y = y;

  float current_x = start_x;
  float current_y = start_y;

  for( int row{}; row < 4; ++row ) {
    for( int column{}; column < 4; ++column ) {
      // Draw the next tetromino.
      const int index = row * 4 + column;
      if( tetromino.current_mask() & ( 1 << index ) ) {
        draw_list->AddRectFilled(
          { current_x, current_y },
          { current_x + GRID_SIZE, current_y + GRID_SIZE },
          m_colours[ m_next_tetromino_idx ],
          4.F
        );
      }

      current_y += GRID_SIZE + GRID_SPACING;
    }

    current_x += GRID_SIZE + GRID_SPACING;
    current_y = start_y;
  }

  draw_list->AddRect(
    { start_x - ( GRID_SPACING * 2.F ), start_y - ( GRID_SPACING * 2.F ) },
    {
      start_x + ( GRID_SIZE + GRID_SPACING ) * rows + GRID_SPACING,
      start_y + ( GRID_SIZE + GRID_SPACING ) * columns + GRID_SPACING
    },
    0x7FFFFFFF,
    4.F
  );
}

void game::Board::physics_start() {
  m_previous_position_x = m_current_position_x;
  m_previous_position_y = m_current_position_y;
}

bool game::Board::physics_rotate( const double t, const double dt ) {
  const bool rotate = ImGui::IsKeyDown( ImGuiKey_R );

  m_next_rotate_time = m_last_rotate_time + ( 6.0 / 60.0 );
  if( t < m_next_rotate_time ) {
    return false;
  }

  bool rotated = false;

  if( rotate && can_rotate() ) {
    rotate_tetromino();
    rotated = true;
  }

  m_last_rotate_time = t;
  return rotated;
}

void game::Board::physics_move( const double t, const double dt ) {
  /*
   https://en.wikipedia.org/wiki/Tetris_(NES_video_game)

   One of the most limiting factors in NES Tetris is the speed at which a tetromino can be moved left and
   right. On the NTSC version, when a movement key is pressed, the piece will instantly move one grid cell,
   stop for 16 frames due to delayed auto-shift, before moving again once every 6 frames[12] (10 times a
   second, as the game runs at 60 fps[13][better source needed]). At higher levels, waiting for this delay
   is not feasible because the pieces fall too fast.
 */

  // I'm going to assume a rotation is considered a move and add the logic into here too.

  const bool left = ImGui::IsKeyDown( ImGuiKey_LeftArrow );
  const bool right = ImGui::IsKeyDown( ImGuiKey_RightArrow );
  
  // If neither movement key is pressed, reset.
  if( !( left || right ) ) {
    m_last_move_time = 0.0;
    m_first_move = true;
    return;
  }

  const double move_delay = m_first_move ? ( 16.0 / 60.0 ) : ( 6.0 / 60.0 );
  m_next_move_time = m_last_move_time + move_delay;

  // Are we allowed to move yet?
  if( t < m_next_move_time ) {
    return;
  }

  //
  // We can now move.
  //
  if( left && can_move_side( -1 ) ) {
    m_current_position_x -= 1;
  }
  else if( right && can_move_side( 1 ) ) {
    m_current_position_x += 1;
  }

  // Until we release all keys again, consider any further moves are repeats.
  m_first_move = false;

  // Increment the last time we moved to the current physics time.
  m_last_move_time = t;
}

bool game::Board::physics_gravity( const double dt ) {
  // How long we're allowed to stay on the current line based on our level.
  //  
  //    The formula is adapted from (https://harddrop.com/wiki/Tetris_Worlds) to work with
  //    our physics running at 60fps.
  //
  const double max_time = 1.0 - ( m_level * 0.07 );
  const double fast_time = ( 2.0 / 60.0 );

  const bool speed_up = ImGui::IsKeyDown( ImGuiKey_S );

  m_time_on_line += dt;
  if( m_time_on_line >= ( speed_up ? fast_time : max_time ) ) {
    if( !can_move_down( *m_curr_tetromino, m_current_position_x, m_current_position_y ) ) {
      new_tetromino();
      m_time_on_line = 0.0;
      return false;
    }

    m_current_position_y = std::min( m_columns, m_current_position_y + 1 );

    if( speed_up ) {
      ++m_score;
    }

    m_time_on_line = 0.0;
  }

  return true;
}

void game::Board::physics( const double t, const double dt ) {
  if( m_game_over ) {
    return;
  }
  
  physics_start();
  physics_rotate( t, dt );
  physics_move( t, dt );
  physics_gravity( dt );
}

void game::Board::update() {
  if( m_game_over ) {
    return;
  }

  clear_prev_tetromino();
  clear_completed_lines();
  draw_tetromino();
}