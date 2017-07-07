uniform mat4  world;
uniform mat4  view;
uniform mat4  projection;

attribute vec2 a_vertex;        // vertex position
attribute vec4 a_diffuse;       // vertex colour

varying vec4 v_diffuse;

void main(void)
{
  // Make it a 4 value vector.
  vec4 vert = vec4( a_vertex, 0.0, 1.0 );

  // Generate the projection view world matrix
  mat4 mvp = world * view * projection;

  // Generate the vertex position
  gl_Position = vert * mvp;

  v_diffuse = a_diffuse;
}