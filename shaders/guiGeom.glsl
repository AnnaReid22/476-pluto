#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 geomPos;
out vec2 center;
out vec2 scale;

void main()
{
    center = gl_in[0].gl_Position.xy;
    scale = gl_in[0].gl_Position.zw;


    geomPos = vec2(center.x + scale.x, center.y + scale.y);
    gl_Position = vec4(geomPos, 0, 1);
    EmitVertex();
    
    geomPos = vec2(center.x + scale.x, center.y - scale.y);
    gl_Position = vec4(geomPos, 0, 1);
    EmitVertex();
    
    geomPos = vec2(center.x - scale.x, center.y + scale.y);
    gl_Position = vec4(geomPos, 0, 1);
    EmitVertex();
    
    geomPos = vec2(center.x - scale.x, center.y - scale.y);
    gl_Position = vec4(geomPos, 0, 1);
    EmitVertex();
    
    EndPrimitive();
}