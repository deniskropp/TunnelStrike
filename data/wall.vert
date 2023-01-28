varying vec4 ver;

void main()
{
    ver = gl_Vertex;

    // Transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
