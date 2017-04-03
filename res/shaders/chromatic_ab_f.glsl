uniform sampler2D texture;

vec3 offset = vec3(-0.005, 0.005, -0.01);

void main()
{
    // lookup the pixel in the texture
    vec4 rval = texture2D(texture, gl_TexCoord[0] + offset.x);
	vec4 gval = texture2D(texture, gl_TexCoord[0] + offset.y);
	vec4 bval = texture2D(texture, gl_TexCoord[0] + offset.z);

    // multiply it by the color
    gl_FragColor = gl_Color * vec4(rval.r, gval.g, bval.b, 1.0);
}
