uniform sampler2D sourceTexture;

float sigma = 0.5;
float glowMultiplier = 1.25;
float width = 800;
float height = 600;

const int KERNEL_SIZE = 5;
float glow = glowMultiplier / (sigma * sqrt(2.0 * 3.14159));

float blurWeight(float x)
{
    return (glow * exp(-(x*x) / (2.0 * sigma * sigma)));
}

void main()
{
    vec4 color = vec4(0.0);
    vec2 texCoord = gl_TexCoord[0].xy;

    for (int i = -KERNEL_SIZE; i <= KERNEL_SIZE; i++)
    {
        texCoord.x = gl_TexCoord[0].x + (i / width);
		texCoord.y = gl_TexCoord[0].y + (i / height);
        color += texture2D(sourceTexture, texCoord) * blurWeight(i);
    }

    gl_FragColor = color;
}
