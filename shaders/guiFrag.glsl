#version 330 core 

uniform float aspect;
uniform vec3 rgb;
uniform float texAlpha;
uniform sampler2D tex;

in vec2 center;
in vec2 scale;
in vec2 geomPos;

out vec4 color;

void main()
{
    vec2 parsedScale = scale;
    float minDist = 0;
    if(scale.x <= 0 && scale.y <= 0){
        discard;
    }else if(scale.x <= 0){
        minDist = -scale.x;
        parsedScale.x = scale.y;
    }else if(scale.y <= 0){
        minDist = -scale.y;
        parsedScale.y = scale.x;
    }

    vec2 normScale = vec2(parsedScale.x, parsedScale.y);
    vec2 normPos = vec2((geomPos.x - center.x) * aspect, geomPos.y - center.y);

    float scaleMin, scaleMax, posMin, posMax;
    if(normScale.x > normScale.y){
        scaleMax = normScale.x;
        scaleMin = normScale.y;
        posMax = normPos.x;
        posMin = normPos.y;
    }else{
        scaleMax = normScale.y;
        scaleMin = normScale.x;
        posMax = normPos.y;
        posMin = normPos.x;
    }

    float dist = abs(posMin);
    if(posMax < scaleMin - scaleMax){
        dist = length(vec2(posMax + scaleMax - scaleMin, posMin));
    }else if(posMax > scaleMax - scaleMin){
        dist = length(vec2(posMax - scaleMax + scaleMin, posMin));
    }
    dist /= scaleMin;
    if(dist > 1 || dist < minDist){
        discard;
    }
    if(texAlpha < 0){
        color = vec4(rgb, 1);
    }else{
        //color = vec4(1, 0, 0, 1);
        color = texture(tex, geomPos * 0.8f + vec2(0.5f)) * texAlpha;
    }
}
