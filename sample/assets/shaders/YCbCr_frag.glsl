#version 110

uniform sampler2D Y;
uniform sampler2D Cr;
uniform sampler2D Cb;


void main(void)
{
    vec3 yCbCr;
    yCbCr.x = texture2D(Y,  gl_TexCoord[0].st).x;
    yCbCr.y = texture2D(Cr, gl_TexCoord[0].st).y;
    yCbCr.z = texture2D(Cb, gl_TexCoord[0].st).z;
    
    gl_FragColor.x = yCbCr.x + 1.140   * ( yCbCr.y - 0.5 );
    gl_FragColor.y = yCbCr.x - 0.34414 * ( yCbCr.z - 0.5 ) - 0.71414 * ( yCbCr.y - 0.5 );
    gl_FragColor.z = yCbCr.x + 1.772   * ( yCbCr.z - 0.5  );

    gl_FragColor.xyz =  (gl_FragColor.xyz - vec3(0.5, 0.5, 0.5)) + vec3(0.5, 0.5, 0.5);
    
}