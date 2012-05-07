
varying vec3 N;
varying vec3 L;
varying vec3 E;
varying vec3 H;

attribute vec4 vertexPosition;
attribute vec3 vertexNormal;

uniform vec4 lightPosition;
uniform mat4 Projection;
uniform mat4 ModelView;

void main()
{
    gl_Position = Projection * ModelView * vertexPosition;

    vec4 eyePosition = ModelView * vertexPosition;
    vec4 eyeLightPos = lightPosition;

    N = normalize(ModelView * vec4(vertexNormal,0)).xyz;
    L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    E = -normalize(eyePosition.xyz);
    H = normalize(L + E);

   
}

