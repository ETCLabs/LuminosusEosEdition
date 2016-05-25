.pragma library

var greyWave = " \
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform float resolution_x;
uniform float resolution_y;
vec2 resolution = vec2(resolution_x, resolution_y);
varying highp vec2 qt_TexCoord0;

// custom shader begin:

float rand(vec2 co){
    return fract(cos(dot(co.xy ,vec2(12.9898,78.233))) * 758.5453);
}

void main( void )
{
    vec2 pos = gl_FragCoord.xy / resolution.xy;
    vec2 uPos = pos;
    uPos.y -= 0.5;

    vec3 color = vec3(0.0);
    const float k = 2.5;
    for( float i = 1.; i < k; i+=0.25 ) {
        float t = time * (1.0);
        uPos.y += sin( uPos.x*exp(i) - t) * 0.045;
        float fTemp = abs(1.0/(80.0*k) / uPos.y);
        color += vec3(fTemp, fTemp, fTemp)/20.;
    }
    vec4 color_final = vec4(color, 1.0);
    gl_FragColor = color_final + 0.2;
    float ft = fract(0.);
    gl_FragColor.rgb += vec3( rand( pos +  7.+ ft ),
          rand( pos +  92.+ ft ),
          rand( pos + 11.+ ft ) ) / 25.0;
}"

var colorWave = " \
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform float resolution_x;
uniform float resolution_y;
vec2 resolution = vec2(resolution_x, resolution_y);

void main( void ) {
    vec2 pos = 1.0 * gl_FragCoord.xy / resolution;
    float amnt;
    float nd;
    vec4 cbuff = vec4(0.0);

    for(float i=0.0; i<2.5;i++){
      nd = sin(3.17*0.8*pos.x + (3.0*i*0.1+sin(+time)*0.2) + time)*0.8+0.1 + pos.x;
      amnt = 1.0/(0.1+abs(nd-1.4*pos.y))*0.101;

      cbuff += vec4(amnt, amnt*0.5 , amnt*pos.y, 90.0);
    }
  gl_FragColor = cbuff;
}"

var sparcleWave = " \
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform float resolution_x;
uniform float resolution_y;
vec2 resolution = vec2(resolution_x, resolution_y);

vec2 hash(vec2 p)
{
    mat2 m = mat2(  1.85, 47.77,
                    99.41, 88.48
                );

    return fract(sin(m*p) * 46738.29);
}

float voronoi(vec2 p)
{
    vec2 g = floor(p);
    vec2 f = fract(p);

    float distanceToClosestFeaturePoint = 1.0;
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 latticePoint = vec2(x, y);
            float currentDistance = distance(latticePoint + hash(g+latticePoint), f);
            distanceToClosestFeaturePoint = min(distanceToClosestFeaturePoint, currentDistance);
        }
    }

    return distanceToClosestFeaturePoint;
}

void main( void )
{
    vec2 uv = ( gl_FragCoord.xy / resolution.xy ) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    float offset = voronoi(uv*70.0 + vec2(time));
    float t = 1.0/abs(((uv.x + sin(uv.y + time)) + offset) * 30.0);

    float r = voronoi( uv * 1.0 ) * 10.0;
    vec3 finalColor = vec3(10.0 * uv.y, 2.0, 1.0 * r) * t;

    gl_FragColor = vec4(finalColor, 1.0 );
}"

var halo = " \
#ifdef GL_ES
precision mediump float;
#endif

float normpdf(in float x, in float sigma)
{
    return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec3 c = texture2D(iChannel0, fragCoord.xy / iResolution.xy).rgb;
    if (fragCoord.x < iMouse.x)
    {
        fragColor = vec4(c, 1.0);
    } else {

        //declare stuff
        const int mSize = 13;
        const int kSize = (mSize-1)/2;
        float kernel[mSize];
        vec3 final_colour = vec3(0.0);

        //create the 1-D kernel
        float sigma = 7.0;
        float Z = 0.0;
        for (int j = 0; j <= kSize; ++j)
        {
            kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
        }

        //get the normalization factor (as the gaussian has been clamped)
        for (int j = 0; j < mSize; ++j)
        {
            Z += kernel[j];
        }

        //read out the texels
        for (int i=-kSize; i <= kSize; ++i)
        {
            for (int j=-kSize; j <= kSize; ++j)
            {
                vec3 ci = texture2D(iChannel0, (fragCoord.xy+vec2(float(i),float(j))) / iResolution.xy).rgb;
                if (max(ci.x, max(ci.y, ci.z)) < 0.8) {
                    continue;
                }
                final_colour += kernel[kSize+j]*kernel[kSize+i]*ci;

            }
        }

        vec3 fc = final_colour/(Z*Z)*0.7;
        fragColor = vec4(fc+c, 1.0);
    }
}"

