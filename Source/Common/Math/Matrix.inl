

//[row][colum]


inline matrix2_t::matrix2_t( float m00, float m01, float m10, float m11 )
{
  m[ 0 ][ 0 ] = m00;
  m[ 0 ][ 1 ] = m01;
  m[ 1 ][ 0 ] = m10;
  m[ 1 ][ 1 ] = m11;
}

inline matrix2_t
matrix2_t::operator* ( const matrix2_t& other )
{
  matrix2_t r;
  r.m[ 0 ][ 0 ] = m[ 0 ][ 0 ] * other.m[ 0 ][ 0 ] + m[ 0 ][ 1 ] * other.m[ 1 ][ 0 ];
  r.m[ 0 ][ 1 ] = m[ 0 ][ 0 ] * other.m[ 0 ][ 1 ] + m[ 0 ][ 1 ] * other.m[ 1 ][ 1 ];

  r.m[ 1 ][ 0 ] = m[ 1 ][ 0 ] * other.m[ 0 ][ 0 ] + m[ 1 ][ 1 ] * other.m[ 1 ][ 0 ];
  r.m[ 1 ][ 1 ] = m[ 1 ][ 0 ] * other.m[ 0 ][ 1 ] + m[ 1 ][ 1 ] * other.m[ 1 ][ 1 ];

  return r;
}

inline vector2_t
matrix2_t::operator* ( const vector2_t& v )
{
  return vector2_t(
    m[ 0 ][ 0 ] * v.x + m[ 0 ][ 1 ] * v.y,
    m[ 1 ][ 0 ] * v.x + m[ 1 ][ 1 ] * v.y
  );
}

inline matrix2_t
matrix2_t::operator* ( const float& s )
{
  return matrix2_t(
    m[ 0 ][ 0 ] * s, m[ 0 ][ 1 ] * s, m[ 1 ][ 0 ] * s, m[ 1 ][ 1 ] * s
  );
}

inline matrix2_t
matrix2_t::operator+ ( const matrix2_t& other )
{
  matrix2_t r(
    m[ 0 ][ 0 ] + other.m[ 0 ][ 0 ], m[ 0 ][ 1 ] + other.m[ 0 ][ 1 ],
    m[ 1 ][ 0 ] + other.m[ 1 ][ 0 ], m[ 1 ][ 1 ] + other.m[ 1 ][ 1 ]
  );
  return r;
}

inline matrix2_t
matrix2_t::operator- ( const matrix2_t& other )
{
  matrix2_t r(
    m[ 0 ][ 0 ] - other.m[ 0 ][ 0 ], m[ 0 ][ 1 ] - other.m[ 0 ][ 1 ],
    m[ 1 ][ 0 ] - other.m[ 1 ][ 0 ], m[ 1 ][ 1 ] - other.m[ 1 ][ 1 ]
  );
  return r;
}