#version 420

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;
 
in VertexData {
  vec3 posWS;
} VertexIn[3];

out VertexData {
    vec3 posWS;
} Out;
 
void main()
{
  for(int i = 0; i < gl_in.length(); i++)
  {
     // copy attributes
    gl_Position = gl_in[i].gl_Position;

    Out.posWS = VertexIn[i].posWS;

    // done with the vertex
    EmitVertex();
  }
  EndPrimitive();
}
