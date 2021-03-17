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
uniform int is_environment;
uniform int is_environment_with_phong;
uniform int is_reflection;
uniform int is_refraction;

uniform Light Lights[NR_POINT_LIGHTS];
uniform Material material;
uniform sampler2D skybox[6];
uniform vec3 GlobalIemissive;
uniform vec3 GlobalAmbientColor;
uniform vec3 FogColor;
uniform vec3 Ka;
uniform float constant;
uniform float linear;
uniform float quadratic;

uniform float near_distance;
uniform float far_distance;
uniform float refraction_ratio_r;
uniform float refraction_ratio_g;
uniform float refraction_ratio_b;

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
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		vec3 reflectDir = normalize(2.f*dot(normal,lightDir)*normal-lightDir);
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		
		float spec = max(dot(viewDir, reflectDir), 0.0);
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
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		vec3 reflectDir = normalize(2.f*dot(normal,lightDir)*normal-lightDir);
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		
		float spec = max(dot(viewDir, reflectDir), 0.0);
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
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		vec3 reflectDir = normalize(2.f*dot(normal,lightDir)*normal-lightDir);
		float ns=vec3(texture(material.specular, texCoord)).x*vec3(texture(material.specular, texCoord)).x;
		float spec = max(dot(viewDir, reflectDir), 0.0);
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

vec3 Decide_Color_by_vector(vec3 vector)
{
    float x = vector.x;
    float y = vector.y;
    float z = vector.z;

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    int isXPositive = x > 0 ? 1 : 0;
    int isYPositive = y > 0 ? 1 : 0;
    int isZPositive = z > 0 ? 1 : 0;

    float maxAxis,uc, vc;
    vec2 uv = vec2(0);

    // POSITIVE X
    if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
    {
        maxAxis = absX;
        uc = z;
        vc = -y;
		
		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		
		uv.s=1-uv.s;
		//return vec3(1,1,1);
		
		return vec3(texture(skybox[3],uv));
    }
	// NEGATIVE X
    else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
    {
        maxAxis = absX;
        uc = -z;
        vc = -y;
				
		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		uv.s=1-uv.s;		
		
		//return vec3(1,0,0);
		
		return vec3(texture(skybox[2],uv));
    }
	// POSITIVE Y
    else if (bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        maxAxis = absY;
        uc = -x;
        vc = -z;
				
		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		
		return vec3(texture(skybox[0],uv));
    }
	// NEGATIVE Y
    else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        maxAxis = absY;
        uc = x;
        vc = -z;

		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		
		return vec3(texture(skybox[1],uv));
    }
	// POSITIVE Z
    else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        maxAxis = absZ;
        uc = -x;
        vc = -y;
				
		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		uv.s=1-uv.s;		
		return vec3(texture(skybox[4],uv));
    }
	// NEGATIVE Z
    else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        maxAxis = absZ;
        uc = x;
        vc = -y;
				
		uv.s = 0.5f * (uc / maxAxis + 1.0f);
		uv.t = 0.5f * (vc / maxAxis + 1.0f);
		uv.s=1-uv.s;		
		return vec3(texture(skybox[5],uv));
    }
}

void main()
{  
    vec3 viewDir = normalize(viewPos - FragPos);
	vec2 texcoord;
	vec3 normal=normalize(Normal);
	vec3 color;
	
	
	if(is_environment)
	{
		vec3 I = normalize(FragPos-viewPos);

		vec3 reflectDir = normalize(2.f*dot(normal,viewDir)*normal-viewDir);
		vec3 reflectColor=Decide_Color_by_vector(reflectDir);
		
		//r
		float k = 1.0 - refraction_ratio_r * refraction_ratio_r * (1.0 - dot(normal, I) * dot(normal, I));
		
		float F=((1.0-refraction_ratio_r)*(1.0-refraction_ratio_r))/((1.0+refraction_ratio_r)*(1.0+refraction_ratio_r));
		float mix_ratio=F+(1.0-F)*pow(1.0-dot(normal,-I),1.0);
		
		vec3 refractionDir= refraction_ratio_r * I - (refraction_ratio_r * dot(normal, I) + sqrt(k)) * normal;
		if(k<0)
		{
			refractionDir=vec3(0,0,0);
		}
		vec3 refractionColor=Decide_Color_by_vector(refractionDir);
		
		//g
		k = 1.0 - refraction_ratio_g * refraction_ratio_g * (1.0 - dot(normal, I) * dot(normal, I));
		
		F=((1.0-refraction_ratio_g)*(1.0-refraction_ratio_g))/((1.0+refraction_ratio_g)*(1.0+refraction_ratio_g));
		mix_ratio=F+(1.0-F)*pow(1.0-dot(normal,-I),1.0);
		
		refractionDir= refraction_ratio_g * I - (refraction_ratio_g * dot(normal, I) + sqrt(k)) * normal;
		if(k<0)
		{
			refractionDir=vec3(0,0,0);
		}
		refractionColor.g=Decide_Color_by_vector(refractionDir).g;
		
		//b
		k = 1.0 - refraction_ratio_b * refraction_ratio_b * (1.0 - dot(normal, I) * dot(normal, I));
		
		F=((1.0-refraction_ratio_b)*(1.0-refraction_ratio_b))/((1.0+refraction_ratio_b)*(1.0+refraction_ratio_b));
		mix_ratio=F+(1.0-F)*pow(1.0-dot(normal,-I),1.0);
		
		refractionDir= refraction_ratio_b * I - (refraction_ratio_b * dot(normal, I) + sqrt(k)) * normal;
		if(k<0)
		{
			refractionDir=vec3(0,0,0);
		}
		refractionColor.b=Decide_Color_by_vector(refractionDir).b;
		
		if(is_reflection==1&&is_refraction==0)
		{
			color=reflectColor;
			FragColor=vec4(color,1.0);
		}
		else if(is_reflection==0&&is_refraction==1)
		{
			color=refractionColor;
			FragColor=vec4(color,1.0);
		}
		else if(is_reflection==1&&is_refraction==1)
		{
			color=mix(reflectColor,refractionColor,mix_ratio);
			FragColor=vec4(color,1.0);
		}
		
	}
	if(is_environment==0||is_environment_with_phong==1)
	{
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
		
		if(is_environment_with_phong==1)
		{
			color=mix(color,result,0.3);
			FragColor  = vec4(color,1.0);
		}
		else
		{
			FragColor  = vec4(result,1.0);
		}
	}
}