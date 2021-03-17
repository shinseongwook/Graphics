#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;

out vec2 TexCoords;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = vTexCoords;
    
	mat3 real_view = mat3( view[0][0], view[0][1], view[0][2], view[1][0], view[1][1], view[1][2], view[2][0], view[2][1], view[2][2]);
	
	vec4 pos = projection * vec4(real_view * vPosition, 1.0f);
	
	//vec4 pos=projection*view*vec4(vPosition,1.0);
	
	
	
    gl_Position = pos.xyww;
}