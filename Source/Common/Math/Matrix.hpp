#ifndef MATH_MATRIX_H__
#define MATH_MATRIX_H__

struct matrix2_t
{
public:
  // The matrix entries, indexed by [row][column].
  union
  {
    float m[ 2 ][ 2 ];
    float _m[ 4 ];
  };

public:
  matrix2_t() {}
  matrix2_t( float m00, float m01, float m10, float m11 );

  matrix2_t operator* ( const matrix2_t& other );
  vector2_t operator* ( const vector2_t& v );
  matrix2_t operator* ( const float& s );

  matrix2_t operator+ ( const matrix2_t& other );
  matrix2_t operator- ( const matrix2_t& other );

};

#endif // MATH_MATRIX_H__

