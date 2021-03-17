#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
	int is_active;
	int is_point;
	int is_directional;
	int is_spotlight;
    vec3 position;
	vec3 direction;
    float cutOff;
    float outerCutOff;
	float falloff;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 16

uniform int is_cpu_calculate;
uniform int is_cube_map;
uniform int is_cylindrical;
uniform int is_spherical;
uniform int is_vertex_pos;

uniform Light Lights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 GlobalIemissive;
uniform vec3 GlobalAmbientColor;
uniform vec3 FogColor;
uniform vec3 Ka;
uniform float constant;
uniform float linear;
uniform float quadratic;

uniform float near_distance;
uniform float far_distance;

uniform vec3 centerPos;
uniform vec3 viewPos;
uniform mat4 vertexTransform;
uniform mat4 view;
uniform mat4 projection;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// calculates the color when using a light.
vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir,vec2 texCoord)
{
	if(light.is_active==0)
		return vec3(0);
	if(bool(light.is_point))
	{
		vec3 lightDir = normalize(light.position - fragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		float spec = max(dot(normal, halfwayDir), 0.0);
		if(spec > 0.0)
			spec = pow(spec,ns);
		// attenuation
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
		// combine results
		vec3 ambient = light.ambient * Ka;
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
		vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
		return (ambient + diffuse + specular);
	}
	if(bool(light.is_directional))
	{
	    vec3 lightDir = normalize(-light.direction);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		float spec = max(dot(normal, halfwayDir), 0.0);
		if(spec > 0.0)
			spec = pow(spec,ns);
		// combine results
		vec3 ambient = light.ambient * Ka;
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
		vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

		return (ambient + diffuse + specular);
	}
	if(bool(light.is_spotlight))
	{
	    vec3 lightDir = normalize(light.position - fragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		float spec = max(dot(normal, halfwayDir), 0.0);
		if(spec > 0.0)
			spec = pow(spec,ns);
		// attenuation
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
		// spotlight intensity
		float theta = dot(lightDir, normalize(-light.direction)); 
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
		intensity = pow(intensity,light.falloff);
		// combine results
		vec3 ambient = light.ambient * Ka;
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
		vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

		ambient *= attenuation;
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;
		return (ambient + diffuse + specular);
	}
}

vec2 CalcCubeMap()
{
    float x = FragPos.x;
    float y = FragPos.y;
    float z = FragPos.z;

	if(is_vertex_pos==0)
	{
		x = Normal.x;
	    y = Normal.y;
        z = Normal.z;
	}

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    int isXPositive = x > 0 ? 1 : 0;
    int isYPositive = y > 0 ? 1 : 0;
    int isZPositive = z > 0 ? 1 : 0;

    float maxAxis, uc, vc;
    vec2 uv = vec2(0);

    // POSITIVE X
    if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
    {
        maxAxis = absX;
        uc = -z;
        vc = y;
    }

        // NEGATIVE X
    else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
    {
        maxAxis = absX;
        uc = z;
        vc = y;
    }

        // POSITIVE Y
    else if (bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        maxAxis = absY;
        uc = x;
        vc = -z;
    }

        // NEGATIVE Y
    else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        maxAxis = absY;
        uc = x;
        vc = z;
    }

        // POSITIVE Z
    else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        maxAxis = absZ;
        uc = x;
        vc = y;
    }

        // NEGATIVE Z
    else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        maxAxis = absZ;
        uc = -x;
        vc = y;
    }

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);
	
	return uv;
}

vec2 CalcCylindrical()
{
	vec3 centroidVec=normalize(FragPos-centerPos);
	
	if(is_vertex_pos==0)
	{
		centroidVec=normalize(Normal-centerPos);
	}
    vec2 uv = vec2(0);
	float theta=degrees(atan(centroidVec.y,centroidVec.x))+180.f;
	float z=(centroidVec.z+1.0f)/2.f;
	
	uv.x=theta/360.f;
	uv.y=z;
	
	return uv;
}

vec2 CalcSpherical()
{
	vec3 centroidVec=normalize(FragPos-centerPos);
	
	if(is_vertex_pos==0)
	{
		centroidVec=normalize(Normal-centerPos);
	}
    vec2 uv = vec2(0);
	float theta=degrees(atan(centroidVec.y,centroidVec.x))+180.f;
	float z=centroidVec.z;
	float phi=degrees(acos(z/centroidVec.length()));
	
	uv.x=theta/360.f;
	uv.y=(1.0f-(phi/180.f))*2.f;
	
	return uv;
}

void main()
{  
    vec3 viewDir = normalize(viewPos - FragPos);
	vec2 texcoord;
	
	if(bool(is_cpu_calculate))
	{
		texcoord=TexCoords;
	}
	else
	{
		if(bool(is_cube_map))
		{
			texcoord=CalcCubeMap();
		}
		else if(bool(is_cylindrical))
		{
			texcoord=CalcCylindrical();
		}
		else if(bool(is_spherical))
		{
			texcoord=CalcSpherical();
		}
	}

	vec3 result = vec3(0);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
        result += CalcLight(Lights[i], Normal, FragPos, viewDir, texcoord);    
	}
	result += GlobalIemissive;
	result += Ka*GlobalAmbientColor;
	
	float S=(far_distance-(viewPos-FragPos).length())/(far_distance-near_distance);
	
	result = S*result+(1-S)*FogColor;
	
	FragColor  = vec4(result,1.0);
}