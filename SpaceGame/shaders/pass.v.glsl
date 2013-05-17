varying vec2 f_texcoord;

void main(void)
{
    f_texcoord = gl_MultiTexCoord0.st;
    gl_Position    = gl_Vertex; 
}

