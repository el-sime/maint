#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform float offset;       // scanline scroll offset in pixels
uniform float lineHeight;   // height of one scanline band in pixels
uniform float screenHeight; // screen height in pixels
uniform float lineAlpha;    // darkness of the dark scanline band [0,1]
uniform float curvature;    // barrel distortion strength (0 = flat)
uniform float vigStrength;  // vignette exponent (lower = stronger, 0.15 default)

out vec4 finalColor;

vec2 barrelDistort(vec2 uv, float k)
{
    vec2 cc = uv - 0.5;
    float r2 = dot(cc, cc);
    return uv + cc * r2 * k;
}

void main()
{
    vec2 uv = barrelDistort(fragTexCoord, curvature);

    // Pixels outside the distorted screen edge become black (CRT border)
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
    {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture(texture0, uv);

    // Scanlines: alternate bright/dark bands scrolling vertically
    float linePos = mod(uv.y * screenHeight + offset, lineHeight * 2.0);
    float scanline = step(lineHeight, linePos) * lineAlpha;
    col.rgb *= (1.0 - scanline);

    // Subtle vignette
    vec2 vig = uv * (1.0 - uv);
    float vignette = clamp(pow(vig.x * vig.y * 15.0, vigStrength), 0.0, 1.0);
    col.rgb *= vignette;

    finalColor = col * fragColor;
}
