/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Graphic.h
Purpose: Definition of Graphic class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include "Camera.h"
#include "Light.h"


class Mesh;
class Camera;

enum class RENDER_TYPE { FAN, STRIP, TRIANGLE, LINES, LINE_LOOP };

enum class SHADER_PROGRAM_TYPE { STANDARD_SHADER,PHONG_LIGHT_SHADER,PHONG_SHADING_SHADER,BLINN_PHONG_SHADER,SKY_BOX_SHADER,FBO_SHADER, MAX_SHADER };

class Graphic
{
public:
	Graphic();
	void Initialize();

	void SetTexture(unsigned int texture_num,std::string file_path,bool is_ppm=false);
	void Setting_Globalvarialbe_Shader();
	void Setting_Globalvarialbe_Shader(int num);

	void Draw_Skybox(Mesh* mesh);
    void Draw_Face(Mesh& mesh, SHADER_PROGRAM_TYPE shader_program_type);
	void Draw_Basic_Face(Mesh& mesh,int light_num);
	void Draw_Lines(Mesh& mesh);
	void Draw_Vertex_Normal(Mesh& mesh);
	void Draw_Face_Normal(Mesh& mesh);
	void Draw_basic_shape_to_FBO(Mesh& mesh);
	void Draw_Skybox_to_FBO(Mesh* mesh);

	void Clear_Buffer();

    void Close();
	void Recompile();

    void InitGLEW();

    unsigned int GetProgramID(SHADER_PROGRAM_TYPE shader_program);

    int GetLocation_CurrentProgram(const char * string);

	void Change_Lights_Number(int nums);
	void Write_LighttoShader();

	void Setting_LightPosDir(int light_num,glm::vec3 pos);
	void Write_ValuetoShader(int shaderprogramnum, std::string variable_name, float value);
	void Write_ValuetoShader(int shaderprogramnum, std::string variable_name, int value);
	void Write_ValuetoShader(int shaderprogramnum, std::string variable_name, glm::vec3 value);
	void Write_ValuetoShader(int shaderprogramnum, std::string variable_name, bool tf);
	void Write_SkyboxtoShader();

	void WritetoAllShader(std::string variable_name, float value);
	void WritetoAllShader(std::string variable_name, int value);
	void WritetoAllShader(std::string variable_name, glm::vec3 value);
	void WritetoAllShader(std::string variable_name, bool tf);



	float* ppmRead(const char* filename, int* width, int* height);


	Camera camera;
	Camera fbo_camera[6];
	Light light[16];
	bool is_cpu_calculate = false;
	bool is_vertex_pos = true;
	bool is_vertex_normal = false;
	bool is_cubemapping = true;
	bool is_cylindrical = false;
	bool is_spherical = false;
	bool is_environment_mapping = false;
	bool is_environment_mapping_with_phong = false;
	bool is_reflection = false;
	bool is_refraction = false;

	float refraction_ratio_r = 1.0f;
	float refraction_ratio_g = 1.0f;
	float refraction_ratio_b = 1.0f;

	glm::vec3 K_ambient=glm::vec3(0.01f);
private:
    int MakeShaderProgram(const char * vertex_source, const char * fragment_source);
    int CompileShader(const char * source, int shader_type);
    void InitShader();
    unsigned int * Shader_Program=nullptr;
    unsigned int Shape_Array=0;
    unsigned int Shape_Buffer[5] = { 0,0,0,0,0 };

	unsigned int texture[8] = {255,255,255,255,255,255,255,255};
	unsigned int fbo;
	unsigned int fbo_texture[6] = { 255,255,255,255,255,255 };
	unsigned int rbo;
};