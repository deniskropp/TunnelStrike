#version 150

in vec4 ver;

void main()
{
    vec4 v = ver;

    v.r = sin(ver.x/100);
    v.g = 1 - sin(ver.x/100);
    v.b = cos(ver.x/100);

    // Read and apply a color from the texture
    gl_FragColor = v;
}
