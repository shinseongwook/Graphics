/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Graphic.cpp
Purpose: Implementation of Graphic class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include <glm/vec3.hpp>
#include <fstream>
#include "Graphic.h"
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "Application.h"
#include <iostream>
#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_i/stb_image.h"

glm::vec3 WHITE(1.f, 1.f, 1.f),
		  PURPLE(1.f, 0, 1.f),
		  BLACK(0, 0, 0),
		  RED(1.f, 0, 0),
		  YELLOW(1.f, 1.f, 0),
		  GREEN(0, 1.f, 0),
		  BLUE(0, 0, 1.f),
		  GREY(0.5f, 0.5f, 0.5f),
		  NOTBLACK(0.1f,0.1f,0.1f);


Graphic::Graphic()
{
	for (int i = 0; i < 16; ++i)
	{
		light[i] = Light(i, this);
	}
}

void Graphic::Initialize()
{
    InitGLEW();

    InitShader();
	Setting_Globalvarialbe_Shader();
	SetTexture(0, "textures/metal_roof_diff_512x512.ppm",true);
	SetTexture(1, "textures/metal_roof_spec_512x512.ppm",true);
	SetTexture(2, "textures/top.tga");
	SetTexture(3, "textures/bottom.tga");
	SetTexture(4, "textures/left.tga");
	SetTexture(5, "textures/right.tga");
	SetTexture(6, "textures/front.tga");
	SetTexture(7, "textures/back.tga");


    glGenVertexArrays(1, &Shape_Array);
	glBindVertexArray(Shape_Array);

    glGenBuffers(1, &Shape_Buffer[0]);//array_buffer for normal shader(vertex)
	glGenBuffers(1, &Shape_Buffer[1]);//array_buffer for normal shader(vertex)
	glGenBuffers(1, &Shape_Buffer[2]);//element_array_buffer for light shader(index)
	glGenBuffers(1, &Shape_Buffer[3]);//array_buffer for light shader (normal)
	glGenBuffers(1, &Shape_Buffer[4]);//array_buffer for light shader (texture)

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (int i = 0; i < 6; ++i)
	{
		glGenTextures(1, &fbo_texture[i]);
		glBindTexture(GL_TEXTURE_2D, fbo_texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, fbo_texture[i], 0);
	}

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 800);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//top
	fbo_camera[0] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.f));
	fbo_camera[0].Front = glm::vec3(0, 1.f, 0);
	fbo_camera[0].WorldUp = glm::vec3(0, 0, -1.f);
	fbo_camera[0].updateCameraVectors();
	//bottom
	fbo_camera[1] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1.f));
	fbo_camera[1].Front = glm::vec3(0, -1.f, 0);
	fbo_camera[1].WorldUp = glm::vec3(0, 0, -1.f);
	fbo_camera[1].updateCameraVectors();
	//left
	fbo_camera[2] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
	fbo_camera[2].Front = glm::vec3(-1, 0, 0.f);
	fbo_camera[2].WorldUp = glm::vec3(0, -1, 0.f);
	fbo_camera[2].updateCameraVectors();
	//right
	fbo_camera[3] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
	fbo_camera[3].Front = glm::vec3(1, 0, 0.f);
	fbo_camera[3].WorldUp = glm::vec3(0, -1, 0.f);
	fbo_camera[3].updateCameraVectors();
	//front
	fbo_camera[4] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
	fbo_camera[4].Front = glm::vec3(0, 0, 1.f);
	fbo_camera[4].WorldUp = glm::vec3(0, -1, 0.f);
	fbo_camera[4].updateCameraVectors();
	//back
	fbo_camera[5] = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 1.f, 0));
	fbo_camera[5].Front = glm::vec3(0, 0, -1.f);
	fbo_camera[5].WorldUp = glm::vec3(0, -1, 0.f);
	fbo_camera[5].updateCameraVectors();

	camera = Camera(glm::vec3(0, 0.0f, -1.0f));
	camera.Front = glm::vec3(0, 0, 1.f);
	camera.WorldUp = glm::vec3(0, 1, 0.f);
	camera.Pitch = 0.f;
	camera.updateCameraVectors();

}

void Graphic::SetTexture(unsigned int texture_num, std::string file_path,bool is_ppm)
{
	glGenTextures(1, &texture[texture_num]);
	glBindTexture(GL_TEXTURE_2D, texture[texture_num]);

	if (is_ppm)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height,channel;
	float* data = nullptr;
	if (is_ppm)
	{
		data = ppmRead(file_path.c_str(), &width, &height);
	}
	else
	{
		data = stbi_loadf(file_path.c_str(), &width, &height, &channel,0);
	}
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	delete data;
	data = nullptr;
}

void Graphic::Setting_Globalvarialbe_Shader()
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		Write_ValuetoShader(Shader_Program[i], "GlobalIemissive", glm::vec3(0.04f, 0.04f, 0.04f));
		Write_ValuetoShader(Shader_Program[i], "GlobalAmbientColor", glm::vec3(0.04f, 0.04f, 0.04f));
		Write_ValuetoShader(Shader_Program[i], "FogColor", glm::vec3(0.5f, 0.5f, 0.5f));
		Write_ValuetoShader(Shader_Program[i], "Ka", K_ambient);

		Write_ValuetoShader(Shader_Program[i], "constant", 1.0f);
		Write_ValuetoShader(Shader_Program[i], "linear", 0.5f);
		Write_ValuetoShader(Shader_Program[i], "quadratic", 0.25f);
		Write_ValuetoShader(Shader_Program[i], "far_distance", 10.f);
		Write_ValuetoShader(Shader_Program[i], "near_distance", 0.1f);

		Write_ValuetoShader(Shader_Program[i], "is_cpu_calculate", is_cpu_calculate);
		Write_ValuetoShader(Shader_Program[i], "is_cube_map", is_cubemapping);
		Write_ValuetoShader(Shader_Program[i], "is_cylindrical", is_cylindrical);
		Write_ValuetoShader(Shader_Program[i], "is_spherical", is_spherical);
		Write_ValuetoShader(Shader_Program[i], "is_vertex_pos", is_vertex_pos);
	}
}

void Graphic::Setting_Globalvarialbe_Shader(int num)
{
	Write_ValuetoShader(Shader_Program[num], "GlobalIemissive", glm::vec3(0.04f, 0.04f, 0.04f));
	Write_ValuetoShader(Shader_Program[num], "GlobalAmbientColor", glm::vec3(0.04f, 0.04f, 0.04f));
	Write_ValuetoShader(Shader_Program[num], "FogColor", glm::vec3(0.5f, 0.5f, 0.5f));
	Write_ValuetoShader(Shader_Program[num], "Ka", K_ambient);

	Write_ValuetoShader(Shader_Program[num], "constant", 1.0f);
	Write_ValuetoShader(Shader_Program[num], "linear", 0.5f);
	Write_ValuetoShader(Shader_Program[num], "quadratic", 0.25f);
	Write_ValuetoShader(Shader_Program[num], "far_distance", 10.f);
	Write_ValuetoShader(Shader_Program[num], "near_distance", 0.1f);
										
	Write_ValuetoShader(Shader_Program[num], "is_cpu_calculate", is_cpu_calculate);
	Write_ValuetoShader(Shader_Program[num], "is_cube_map", is_cubemapping);
	Write_ValuetoShader(Shader_Program[num], "is_cylindrical", is_cylindrical);
	Write_ValuetoShader(Shader_Program[num], "is_spherical", is_spherical);
	Write_ValuetoShader(Shader_Program[num], "is_vertex_pos", is_vertex_pos);

	Write_ValuetoShader(Shader_Program[num], "is_environment", is_environment_mapping);
	Write_ValuetoShader(Shader_Program[num], "is_environment_with_phong", is_environment_mapping_with_phong);
	Write_ValuetoShader(Shader_Program[num], "is_reflection", is_reflection);
	Write_ValuetoShader(Shader_Program[num], "is_refraction", is_refraction);
}

void Graphic::Draw_Skybox(Mesh* mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int program_id = static_cast<int>(SHADER_PROGRAM_TYPE::SKY_BOX_SHADER);
	glUseProgram(Shader_Program[program_id]);
	glDepthFunc(GL_LEQUAL);
	
	for (int i = 0; i < 6; ++i)
	{
		glActiveTexture(GL_TEXTURE2 + i);
		glBindTexture(GL_TEXTURE_2D, static_cast<int>(texture[2 + i]));
		WritetoAllShader("skybox", 2 + i);
		WritetoAllShader("camera_pos", camera.Position);

		glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh->GetVerticesSize() * sizeof(GLfloat) * 3, mesh->GetVertices().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Shape_Buffer[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndices().size() * sizeof(unsigned int)
			, mesh->GetIndices().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[4]);
		glBufferData(GL_ARRAY_BUFFER, mesh->GetTextureCoordSize() * sizeof(GLfloat) * 2, mesh->GetTextureCoord().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		//

		int size = static_cast<int>(mesh->GetIndices().size());

		GLint vProjectionLoc = glGetUniformLocation(Shader_Program[program_id], "projection");
		GLint vViewLoc = glGetUniformLocation(Shader_Program[program_id], "view");

		// Draw the triangle !
		// T * R * S * Vertex
		glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);
		glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
		glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);


		switch (mesh->GetRenderType())
		{
		case RENDER_TYPE::FAN:
			glDrawElements(GL_TRIANGLE_FAN, size, GL_UNSIGNED_INT, 0);
			break;
		case RENDER_TYPE::STRIP:
			glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_INT, 0);
			break;
		case RENDER_TYPE::TRIANGLE:
			glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
			break;
		case RENDER_TYPE::LINES:
			glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
			break;
		case RENDER_TYPE::LINE_LOOP:
			glDrawElements(GL_LINE_LOOP, size, GL_UNSIGNED_INT, 0);
			break;
		default:
			break;
		}
		mesh++;
	}
	glDepthFunc(GL_LESS);
}

void Graphic::Draw_Face(Mesh& mesh, SHADER_PROGRAM_TYPE shader_program_type)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int program_id = static_cast<int>(shader_program_type);
	glUseProgram(Shader_Program[program_id]);
	Setting_Globalvarialbe_Shader(program_id);
	Write_LighttoShader();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, static_cast<int>(texture[1]));
	WritetoAllShader("material.specular", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, static_cast<int>(texture[0]));
	WritetoAllShader("material.diffuse", 0);

	if (is_environment_mapping)
	{
		Write_SkyboxtoShader();
		WritetoAllShader("refraction_ratio_r", refraction_ratio_r);
		WritetoAllShader("refraction_ratio_g", refraction_ratio_g);
		WritetoAllShader("refraction_ratio_b", refraction_ratio_b);
	}

	if (mesh.GetNeedCalculateUV())
	{
		if (is_cubemapping)
		{
			mesh.SetTextureCoord(TEXTURE_MAPPING_TYPE::CUBE_MAPPING, is_vertex_pos);
		}
		else if (is_cylindrical)
		{
			mesh.SetTextureCoord(TEXTURE_MAPPING_TYPE::CYLINDRICAL_MAPPING, is_vertex_pos);
		}
		else if (is_spherical)
		{
			mesh.SetTextureCoord(TEXTURE_MAPPING_TYPE::SPHERICAL_MAPPING, is_vertex_pos);
		}
		mesh.SetNeedCalculateUV(false);
	}

	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesSize() * sizeof(GLfloat) * 3, mesh.GetVertices().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Shape_Buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size()*sizeof(unsigned int)
				,mesh.GetIndices().data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVertexNormalSize() * sizeof(GLfloat)*3, mesh.GetVertexNormal().data(), GL_STATIC_DRAW);
    
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[4]);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetTextureCoordSize() * sizeof(GLfloat) * 2, mesh.GetTextureCoord().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

    int size = static_cast<int>(mesh.GetIndices().size());

	WritetoAllShader("centerPos", mesh.GetTranslate());
	GLint vProjectionLoc = glGetUniformLocation(Shader_Program[program_id], "projection");
	GLint vTransformLoc = glGetUniformLocation(Shader_Program[program_id], "vertexTransform");
	GLint vViewLoc = glGetUniformLocation(Shader_Program[program_id], "view");
	GLint vViewposLoc = glGetUniformLocation(Shader_Program[program_id], "viewPos");
	GLint vColorLoc = glGetUniformLocation(Shader_Program[program_id], "objectColor");


	// Draw the triangle !
	// T * R * S * Vertex
	glm::mat4x4 modelMat = mesh.GetTRSMatrix();
	glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), 1.f, 0.1f, 10.0f);
	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
	glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);

	glUniform3fv(vViewposLoc, 1, &camera.Position[0]);
	glUniform3fv(vColorLoc, 1, &WHITE[0]);


    switch (mesh.GetRenderType())
    {
    case RENDER_TYPE::FAN:
		glDrawElements(GL_TRIANGLE_FAN, size,GL_UNSIGNED_INT,0);
        break;
    case RENDER_TYPE::STRIP:
		glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_INT, 0);
        break;
    case RENDER_TYPE::TRIANGLE:
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
        break;
    case RENDER_TYPE::LINES:
		glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
        break;
    case RENDER_TYPE::LINE_LOOP:
		glDrawElements(GL_LINE_LOOP, size, GL_UNSIGNED_INT, 0);
        break;
    default:
        break;
    }
}

void Graphic::Draw_Basic_Face(Mesh& mesh,int light_num)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(Shader_Program[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesSize() * sizeof(GLfloat) * 3, mesh.GetVertices().data(), GL_STATIC_DRAW);

	//glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Shape_Buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(unsigned int)
		, mesh.GetIndices().data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, mesh.GetVertexNormalSize() * sizeof(GLfloat) * 3, mesh.GetVertexNormal().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	int size = static_cast<int>(mesh.GetIndices().size());

	GLint vTransformLoc = glGetUniformLocation(Shader_Program[0], "vertexTransform");
	GLint vViewLoc = glGetUniformLocation(Shader_Program[0], "view");
	GLint vColorLoc = glGetUniformLocation(Shader_Program[0], "objectColor");
	GLint vProjectionLoc = glGetUniformLocation(Shader_Program[0], "projection");
	// Draw the triangle !
	// T * R * S * Vertex
	glm::mat4x4 modelMat = mesh.GetTRSMatrix();
	glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);

	glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);
	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
	glUniform3fv(vColorLoc, 1, &light[light_num].diffuse[0]);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
}

void Graphic::Draw_Lines(Mesh& mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(Shader_Program[0]);
	int size = mesh.GetVerticesSize();
	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat) * 3, mesh.GetVertices().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	GLint vProjectionLoc = glGetUniformLocation(Shader_Program[0], "projection");
	GLint vTransformLoc = glGetUniformLocation(Shader_Program[0], "vertexTransform");
	GLint vViewLoc = glGetUniformLocation(Shader_Program[0], "view");
	GLint vColorLoc = glGetUniformLocation(Shader_Program[0], "objectColor");

	// T * R * S * Vertex
	glm::mat4x4 modelMat = mesh.GetTRSMatrix();
	glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);
	glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);

	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
	glUniform3fv(vColorLoc, 1, &RED[0]);

	glDrawArrays(GL_LINES, 0, size);
}

void Graphic::Draw_Vertex_Normal(Mesh& mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(Shader_Program[0]);
	int size = mesh.GetVertexNormalLineSize();
	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat) * 3, mesh.GetVertexNormalLine().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	GLint vProjectionLoc = glGetUniformLocation(Shader_Program[0], "projection");
	GLint vTransformLoc = glGetUniformLocation(Shader_Program[0], "vertexTransform");
	GLint vViewLoc = glGetUniformLocation(Shader_Program[0], "view");
	GLint vColorLoc = glGetUniformLocation(Shader_Program[0], "objectColor");

	// T * R * S * Vertex
	glm::mat4x4 modelMat = mesh.GetTRSMatrix();
	glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);
	
	glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);
	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
	glUniform3fv(vColorLoc, 1, &GREEN[0]);

	glDrawArrays(GL_LINES, 0, size);
}

void Graphic::Draw_Face_Normal(Mesh& mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(Shader_Program[0]);
	int size = mesh.GetFaceNormalLineSize();
	glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat) * 3, mesh.GetFaceNormalLine().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLint vProjectionLoc = glGetUniformLocation(Shader_Program[0], "projection");
	GLint vTransformLoc = glGetUniformLocation(Shader_Program[0], "vertexTransform");
	GLint vViewLoc = glGetUniformLocation(Shader_Program[0], "view");
	GLint vColorLoc = glGetUniformLocation(Shader_Program[0], "objectColor");

	// T * R * S * Vertex
	glm::mat4x4 modelMat = mesh.GetTRSMatrix();
	glm::mat4x4 projection_Mat = glm::perspective(glm::radians(camera.Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);

	glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);
	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
	glUniform3fv(vColorLoc, 1, &RED[0]);

	glDrawArrays(GL_LINES, 0, size);
}

void Graphic::Draw_basic_shape_to_FBO(Mesh& mesh)
{
	glUseProgram(Shader_Program[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < 6; ++i)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0+i);
		glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetVerticesSize() * sizeof(GLfloat) * 3, mesh.GetVertices().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Shape_Buffer[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndices().size() * sizeof(unsigned int)
			, mesh.GetIndices().data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[3]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetVertexNormalSize() * sizeof(GLfloat) * 3, mesh.GetVertexNormal().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		int size = static_cast<int>(mesh.GetIndices().size());

		GLint vTransformLoc = glGetUniformLocation(Shader_Program[0], "vertexTransform");
		GLint vViewLoc = glGetUniformLocation(Shader_Program[0], "view");
		GLint vColorLoc = glGetUniformLocation(Shader_Program[0], "objectColor");
		GLint vProjectionLoc = glGetUniformLocation(Shader_Program[0], "projection");
		// Draw the triangle !
		// T * R * S * Vertex
		glm::mat4x4 modelMat = mesh.GetTRSMatrix();
		glm::mat4x4 projection_Mat = glm::perspective(glm::radians(fbo_camera[i].Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);

		glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);
		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &fbo_camera[i].GetViewMatrix()[0][0]);

		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
	}
	glEnable(GL_DEPTH_TEST);
}

void Graphic::Draw_Skybox_to_FBO(Mesh* mesh)
{
	int program_id = static_cast<int>(SHADER_PROGRAM_TYPE::SKY_BOX_SHADER);
	glUseProgram(Shader_Program[program_id]);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Mesh* skybox_start = mesh;
	for (int i = 0; i < 6; ++i)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glDepthFunc(GL_LEQUAL);

			glActiveTexture(GL_TEXTURE2 + i);
			glBindTexture(GL_TEXTURE_2D, static_cast<int>(texture[2 + i]));
			WritetoAllShader("skybox", 2 + i);

			glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[1]);
			glBufferData(GL_ARRAY_BUFFER, mesh->GetVerticesSize() * sizeof(GLfloat) * 3, mesh->GetVertices().data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Shape_Buffer[2]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndices().size() * sizeof(unsigned int)
				, mesh->GetIndices().data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, Shape_Buffer[4]);
			glBufferData(GL_ARRAY_BUFFER, mesh->GetTextureCoordSize() * sizeof(GLfloat) * 2, mesh->GetTextureCoord().data(), GL_STATIC_DRAW);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(1);
			//

			int size = static_cast<int>(mesh->GetIndices().size());

			GLint vProjectionLoc = glGetUniformLocation(Shader_Program[program_id], "projection");
			GLint vViewLoc = glGetUniformLocation(Shader_Program[program_id], "view");

			// Draw the triangle !
			// T * R * S * Vertex
			glm::mat4x4 projection_Mat = glm::perspective(glm::radians(fbo_camera[i].Zoom), (float)APP->Width / (float)APP->Height, 0.1f, 10.0f);
			glUniformMatrix4fv(vViewLoc, 1, GL_FALSE, &fbo_camera[i].GetViewMatrix()[0][0]);
			glUniformMatrix4fv(vProjectionLoc, 1, GL_FALSE, &projection_Mat[0][0]);


			switch (mesh->GetRenderType())
			{
			case RENDER_TYPE::FAN:
				glDrawElements(GL_TRIANGLE_FAN, size, GL_UNSIGNED_INT, 0);
				break;
			case RENDER_TYPE::STRIP:
				glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_INT, 0);
				break;
			case RENDER_TYPE::TRIANGLE:
				glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
				break;
			case RENDER_TYPE::LINES:
				glDrawElements(GL_LINES, size, GL_UNSIGNED_INT, 0);
				break;
			case RENDER_TYPE::LINE_LOOP:
				glDrawElements(GL_LINE_LOOP, size, GL_UNSIGNED_INT, 0);
				break;
			default:
				break;
			}
			mesh++;
		//mesh = skybox_start;
		glDepthFunc(GL_LESS);
	}
}

void Graphic::Clear_Buffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i = 0; i < 6; ++i)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphic::Close()
{
    glDeleteProgram(Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::STANDARD_SHADER)]);
	glDeleteProgram(Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::PHONG_LIGHT_SHADER)]);
	glDeleteProgram(Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER)]);
	glDeleteProgram(Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::BLINN_PHONG_SHADER)]);

    delete[]Shader_Program;
	Shader_Program = nullptr;
}

void Graphic::Recompile()
{
	Close();
	InitShader();
}

void Graphic::InitGLEW()
{
    glewExperimental = static_cast<GLboolean>(true);;
    GLenum errorCode = glewInit();
    if (GLEW_OK != errorCode)
    {
        std::cerr << "Error: GLEW - " << glewGetErrorString(errorCode) << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    if (!GLEW_VERSION_3_3)
    {
        std::cerr << "Error: OpenGL 3.3 API is not available." << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
}

unsigned Graphic::GetProgramID(SHADER_PROGRAM_TYPE shader_program)
{
    return Shader_Program[static_cast<int>(shader_program)];
}

int Graphic::GetLocation_CurrentProgram(const char* string)
{
    int programid = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programid);
    if(programid == 0)
    {
        std::cerr << "Ther is no CurrentProgram\n";
    }
    return glGetUniformLocation(programid, string);
}

void Graphic::Change_Lights_Number(int nums)
{
	for (int i = 0; i < 16; ++i)
	{
		if (i < nums)
		{
			light[i].is_active = true;
		}
		else
		{
			light[i].is_active = false;
		}
	}
}

void Graphic::Write_LighttoShader()
{
	for (int i = 0; i < 16; ++i)
	{
		light[i].WritetoShader();
	}
}

void Graphic::Setting_LightPosDir(int light_num,glm::vec3 pos)
{
	light[light_num].pos = pos;
	light[light_num].direction = -pos;
}

void Graphic::Write_ValuetoShader(int shaderprogramnum, std::string variable_name, float value)
{
	GLint Loc = glGetUniformLocation(shaderprogramnum, variable_name.c_str());
	glUniform1f(Loc, value);
}

void Graphic::Write_ValuetoShader(int shaderprogramnum, std::string variable_name, int value)
{
	GLint Loc = glGetUniformLocation(shaderprogramnum, variable_name.c_str());
	glUniform1i(Loc, value);
}

void Graphic::Write_ValuetoShader(int shaderprogramnum, std::string variable_name, glm::vec3 value)
{
	GLint Loc = glGetUniformLocation(shaderprogramnum, variable_name.c_str());
	glUniform3fv(Loc, 1, &value[0]);
}

void Graphic::Write_ValuetoShader(int shaderprogramnum, std::string variable_name, bool tf)
{
	GLint Loc = glGetUniformLocation(shaderprogramnum, variable_name.c_str());
	glUniform1i(Loc, (int)tf);
}

void Graphic::Write_SkyboxtoShader()
{
	for (int i = 0; i < 6; ++i)
	{
		glActiveTexture(GL_TEXTURE8 + i);
		glBindTexture(GL_TEXTURE_2D, static_cast<int>(fbo_texture[i]));
		WritetoAllShader("skybox["+std::to_string(i)+"]", 8+i);
	}
}

void Graphic::WritetoAllShader(std::string variable_name, float value)
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		GLint Loc = glGetUniformLocation(Shader_Program[i], variable_name.c_str());
		glUniform1f(Loc, value);
	}
}

void Graphic::WritetoAllShader(std::string variable_name, int value)
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		GLint Loc = glGetUniformLocation(Shader_Program[i], variable_name.c_str());
		glUniform1i(Loc, value);
	}
}

void Graphic::WritetoAllShader(std::string variable_name, glm::vec3 value)
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		GLint Loc = glGetUniformLocation(Shader_Program[i], variable_name.c_str());
		glUniform3fv(Loc, 1, &value[0]);
	}
}

void Graphic::WritetoAllShader(std::string variable_name, bool tf)
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		GLint Loc = glGetUniformLocation(Shader_Program[i], variable_name.c_str());
		glUniform1i(Loc, (int)tf);
	}
}

int Graphic::MakeShaderProgram(const char * vertex_source, const char * fragment_source)
{
	int program_number = glCreateProgram();

    GLuint vertexShader = CompileShader(vertex_source, GL_VERTEX_SHADER);

    GLuint fragmentShader = CompileShader(fragment_source, GL_FRAGMENT_SHADER);


    glAttachShader(program_number, vertexShader);
    glAttachShader(program_number, fragmentShader);
    glLinkProgram(program_number);
	glDetachShader(program_number, vertexShader);
	glDetachShader(program_number, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint result;
    GLchar errorLog[512] = {0};

    glGetProgramiv(program_number, GL_LINK_STATUS, &result);
    if (!result) 
    {
        glGetProgramInfoLog(program_number, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program\n" << errorLog << std::endl;
    }

    return program_number;
}

int Graphic::CompileShader(const char* source, int shader_type)
{
    GLuint shader_id = glCreateShader(shader_type);
	// Read the Vertex Shader code from the file
	std::string ShaderCode;
	std::ifstream ShaderStream(source, std::ios::in);
	if (ShaderStream.is_open()) {
		std::string Line;
		while (getline(ShaderStream, Line))
			ShaderCode += "\n" + Line;
		ShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", source);
		std::cout<<getchar();
		return 0;
	}
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader
	printf("Compiling shader : %s\n", source);
	char const* SourcePointer = ShaderCode.c_str();
	glShaderSource(shader_id, 1, &SourcePointer, nullptr);
	glCompileShader(shader_id);

	// Check Shader
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shader_id, InfoLogLength, nullptr, &ShaderErrorMessage[0]);
		printf("%s\n", &ShaderErrorMessage[0]);
	}

    return shader_id;
}

void Graphic::InitShader()
{
    Shader_Program = new unsigned int[static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER)];
    Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::STANDARD_SHADER)] = MakeShaderProgram("shaders/basic_shader.vert",
		"shaders/basic_shader.frag");
	Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::PHONG_LIGHT_SHADER)] = MakeShaderProgram("shaders/phong_light_shader.vert",
		"shaders/phong_light_shader.frag");
	Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER)] = MakeShaderProgram("shaders/phong_shading_shader.vert",
		"shaders/phong_shading_shader.frag");
	Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::BLINN_PHONG_SHADER)] = MakeShaderProgram("shaders/blinn_shading_shader.vert",
		"shaders/blinn_shading_shader.frag");
	Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::SKY_BOX_SHADER)] = MakeShaderProgram("shaders/sky_box_shader.vert",
		"shaders/sky_box_shader.frag");
	Shader_Program[static_cast<int>(SHADER_PROGRAM_TYPE::FBO_SHADER)] = MakeShaderProgram("shaders/fbo_shader.vert",
		"shaders/fbo_shader.frag");
}

float* Graphic::ppmRead(const char* filename, int* width, int* height)
{
	FILE* fp;
	int w, h, tmpint;
	float* image;
	char head[70];		// max line <= 70 in PPM (per spec).

	fopen_s(&fp, filename, "rb");
	if (!fp) {
		perror(filename);
		return NULL;
	}

	fscanf_s(fp, "P%d\n", &tmpint);

	if (tmpint != 6 && tmpint != 3)
	{
		printf("Input file is not ppm. Exiting.\n");
		exit(1);
	}

	// skip comments embedded in header

	fgets(head, 70, fp);
	while (head[0] == '#')
		fgets(head, 70, fp);

	// read image dimensions 

	sscanf_s(head, "%d %d", &w, &h);
	fgets(head, 70, fp);
	sscanf_s(head, "%d", &tmpint);

	if (tmpint != 255)
		printf("Warning: maxvalue is not 255 in ppm file\n");

	image = new float[w * h * 3];

	int i = 0;
	float rgb[3];
	while (i < w * h)
	{
		fscanf_s(fp, "%f %f %f\n", &rgb[0], &rgb[1], &rgb[2]);
		image[i * 3 + 0] = rgb[0] / 255.f;
		image[i * 3 + 1] = rgb[1] / 255.f;
		image[i * 3 + 2] = rgb[2] / 255.f;
		i += 1;
	}
	fclose(fp);

	*width = w;
	*height = h;
	return image;
}
