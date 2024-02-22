#pragma once

#include <cstdint>
#include <cstdio>

// https://en.wikipedia.org/wiki/Tetromino

namespace game {
  
  const size_t NUM_TETROMINO = 7;

  class Tetromino {
    //
    // Designed to fit into a 4x4 grid.
    //
    
    //
    // Position data wont be stored in this class, this class simply defines
    // the possible orientations for a given tetromino.
    //

  private:
    // The maximum number of masks that any single tetromino can have is 4, the min. is 1.
    int     m_masks[ 4 ];
    size_t  m_num_masks;

    int     m_curr_mask;
    int     m_prev_mask;

    size_t  m_curr_idx;

    int     m_width;
    int     m_height;

  private:
    void calculate_width();
    void calculate_height();

  protected:
    void add_mask( const int mask );

  public:
    Tetromino();
    Tetromino( const Tetromino& other );

  public:
    const int current_mask() const;
    const int previous_mask() const;
    const int next_mask() const;

    void reset();
    void rotate();

    // Returns the shapes width.
    const int width() const;

    // Returns the shapes height.
    const int height() const;

    // Returns the row index of the first cell with a set bit given a columm.
    int row_start( const int column );

    // Returns the row index of the last cell with a set bit given a columm.
    int row_end( const int column );

    // Returns the height of a column given a row index.
    const int column_height( const int row ) const;
  };

  class IShape : public Tetromino {
  public:
    IShape() {
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 ) );
      add_mask( ( 1 << 0 | 1 << 4 | 1 << 8 | 1 << 12 ) );
    }
  };

  class OShape : public Tetromino {
  public:
    OShape() {
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 4 | 1 << 5 ) );
    }
  };

  class TShape : public Tetromino {
  public:
    TShape() {
      add_mask( ( 1 << 0 | 1 << 4 | 1 << 5 | 1 << 8 ) );
      add_mask( ( 1 << 1 | 1 << 5 | 1 << 4 | 1 << 9 ) );
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 5 ) );
      add_mask( ( 1 << 4 | 1 << 1 | 1 << 5 | 1 << 6 ) );
    }
  };

  class JShape : public Tetromino {
  public:
    JShape() {
      add_mask( ( 1 << 4 | 1 << 5 | 1 << 6 | 1 << 2 ) );
      add_mask( ( 1 << 0 | 1 << 4 | 1 << 8 | 1 << 9 ) );
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 6 ) );
      add_mask( ( 1 << 1 | 1 << 5 | 1 << 9 | 1 << 0 ) );
    }
  };

  class LShape : public Tetromino {
  public:
    LShape() {
      add_mask( ( 1 << 4 | 1 << 5 | 1 << 6 | 1 << 0 ) );
      add_mask( ( 1 << 1 | 1 << 5 | 1 << 9 | 1 << 8 ) );
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 6 ) );
      add_mask( ( 1 << 1 | 1 << 4 | 1 << 8 | 1 << 0 ) );
    }
  };

  class SShape : public Tetromino {
  public:
    SShape() {
      add_mask( ( 1 << 1 | 1 << 4 | 1 << 5 | 1 << 8 ) );
      add_mask( ( 1 << 0 | 1 << 1 | 1 << 5 | 1 << 6 ) );
    }
  };

  class ZShape : public Tetromino {
  public:
    ZShape() {
      add_mask( ( 1 << 0 | 1 << 4 | 1 << 5 | 1 << 9 ) );
      add_mask( ( 1 << 1 | 1 << 2 | 1 << 4 | 1 << 5 ) );
    }
  };
}