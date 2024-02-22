#include <game/shape.hpp>

#include <cstdio>

game::Tetromino::Tetromino() :
  m_masks{}, 
  m_num_masks( 0 ),
  m_curr_mask( 0xFFFFFFFF ),
  m_prev_mask( 0 ),
  m_curr_idx( 0 ),
  m_width( 0 ),
  m_height( 0 ) {}

game::Tetromino::Tetromino( const Tetromino& other ) : Tetromino() {
  // m_masks, m_num_masks handled by calling add_mask(..)
  for( int i{}; i < other.m_num_masks; ++i ) {
    add_mask( other.m_masks[ i ] );
  }

  m_curr_idx = other.m_curr_idx;
  m_prev_mask = other.m_prev_mask;
  m_curr_mask = other.m_curr_mask;

  // These values are calculated by add_mask but may not be correct for the given rotation.
  m_width = other.m_width;
  m_height = other.m_height;
}

void game::Tetromino::add_mask( const int mask ) {
  if( m_num_masks >= 4 ) {
    return;
  }

  m_masks[ m_num_masks++ ] = mask;
  m_curr_mask = m_masks[ m_curr_idx ];

  calculate_width();
  calculate_height();
}

const int game::Tetromino::current_mask() const {
  return m_curr_mask;
}

const int game::Tetromino::previous_mask() const {
  return m_prev_mask;
}

const int game::Tetromino::next_mask() const {
  return m_masks[ ( m_curr_idx + 1 ) % m_num_masks ];
}

void game::Tetromino::reset() {
  m_curr_idx = 0;
  m_curr_mask = m_masks[ m_curr_idx ];
  m_prev_mask = 0;

  calculate_width();
  calculate_height();
}

void game::Tetromino::rotate() {
  if( m_num_masks == 0 ) {
    return;
  }

  // Increment the current mask index and use modulo to wrap back around once we reach the end.
  m_curr_idx = ++m_curr_idx % m_num_masks;

  // Store the current mask as the previous mask so we can reference it after a rotation.
  m_prev_mask = m_curr_mask;

  // Update the current mask.
  m_curr_mask = m_masks[ m_curr_idx ];

  calculate_width();
  calculate_height();
}

void game::Tetromino::calculate_width() {
  m_width = 0;

  for( int row{}; row < 4; ++row ) {
    for( int column{}; column < 4; ++column ) {
      const int index = row * 4 + column;
      if( ( m_curr_mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      if( row > m_width ) {
        m_width = row;
      }
    }
  }

  m_width = m_width + 1;
}

void game::Tetromino::calculate_height() {
  m_height = 0;

  for( int row{}; row < 4; ++row ) {
    for( int column{}; column < 4; ++column ) {
      const int index = row * 4 + column;
      if( ( m_curr_mask & ( 1 << index ) ) == 0 ) {
        continue;
      }

      if( column > m_height ) {
        m_height = column;
      }
    }
  }

  m_height = m_height + 1;
}

const int game::Tetromino::width() const {
  return m_width;
}

const int game::Tetromino::height() const {
  return m_height;
}

int game::Tetromino::row_start( const int column ) {
  for( int row{}; row < 4; ++row ) {
    const int index = row * 4 + column;
    if( ( m_curr_mask & ( 1 << index ) ) == 0 ) {
      continue;
    }

    return row;
  }

  return -1;
}

int game::Tetromino::row_end( const int column ) {
  int width = 0;

  for( int row{}; row < 4; ++row ) {
    const int index = row * 4 + column;
    if( ( m_curr_mask & ( 1 << index ) ) == 0 ) {
      continue;
    }

    if( row > width ) {
      width = row;
    }
  }

  return width;
}

const int game::Tetromino::column_height( const int row ) const {
  int height = 0;

  for( int column{}; column < 4; ++column ) {
    const int index = row * 4 + column;
    if( ( m_curr_mask & ( 1 << index ) ) == 0 ) {
      continue;
    }

    if( column > height ) {
      height = column;
    }
  }

  return height;
}
