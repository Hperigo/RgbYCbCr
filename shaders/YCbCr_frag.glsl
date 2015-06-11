#version 110

uniform sampler2D Y;
uniform sampler2D Cr;
uniform sampler2D Cb;


void main(void)
{
    
    vec3 yCbCr;
    yCbCr.x = texture2D(Y,  gl_TexCoord[0].st).x;
    yCbCr.y = texture2D(Cb, gl_TexCoord[0].st).y - 0.5;
    yCbCr.z = texture2D(Cr, gl_TexCoord[0].st).z - 0.5;
    
    gl_FragColor.r = dot( vec3( 1.0,  0.0,      1.402 ),    yCbCr );
    gl_FragColor.g = dot( vec3( 1.0 , -0.34414 , -0.71414 ), yCbCr );
    gl_FragColor.b = dot( vec3( 1.0,  1.772,    0.0 ),      yCbCr );
    
}