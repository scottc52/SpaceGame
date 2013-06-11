#define USE_INNER_AND_OUTER_CONES

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

#if defined(USE_INNER_AND_OUTER_CONES)
    uniform float cosOuterCone;
    uniform float cosInnerCone;
#endif

uniform bool enableParallax;
uniform float scale;
uniform float bias;

varying vec3 lightDir;
varying vec3 spotDir;
varying vec3 viewDir;

void main()
{
    vec2 newTexCoord;
    vec3 v = normalize(viewDir);

    if (enableParallax == true)
    {
        float height = texture2D(heightMap, gl_TexCoord[0].st).r;
        
        height = height * scale + bias;
        newTexCoord = gl_TexCoord[0].st + (height * v.xy);
    }
    else
    {
        newTexCoord = gl_TexCoord[0].st;
    }

    vec3 l = lightDir;
    float atten = max(0.0, 1.0 - dot(l, l));
    
    l = normalize(l);
    
    float spotDot = dot(-l, normalize(spotDir));

#if defined(USE_INNER_AND_OUTER_CONES)
    float spotEffect = smoothstep(cosOuterCone, cosInnerCone, spotDot);
#else
    float spotEffect = (spotDot < gl_LightSource[0].spotCosCutoff)
                        ? 0.0 : pow(spotDot, gl_LightSource[0].spotExponent);
#endif

    atten *= spotEffect;

    vec3 n = normalize(texture2D(normalMap, newTexCoord).rgb * 2.0 - 1.0);
    vec3 h = normalize(l + v);

    float nDotL = max(0.0, dot(n, l));
    float nDotH = max(0.0, dot(n, h));
    float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
    
    vec4 ambient = gl_FrontLightProduct[0].ambient * atten;
    vec4 diffuse = gl_FrontLightProduct[0].diffuse * nDotL * atten;
    vec4 specular = gl_FrontLightProduct[0].specular * power * atten;
    vec4 color = gl_FrontLightModelProduct.sceneColor + ambient + diffuse + specular;
    
    gl_FragColor = color * texture2D(colorMap, newTexCoord);
}