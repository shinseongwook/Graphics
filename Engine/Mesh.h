/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Definition of Input class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#pragma once
#include <vector>
#include <string>
#include "Graphic.h"

enum class TEXTURE_MAPPING_TYPE {CUBE_MAPPING,CYLINDRICAL_MAPPING,SPHERICAL_MAPPING};

class Mesh
{
public:
	Mesh();
    void MakeCircle();
	void MakeQuad();
	void MakeSphere(float radius, int numDivisions);
	void MakeSkybox();
	void Clear_Mesh();
	void Load_Objfile(std::string str);
	void SetTranslate(glm::vec3 Translate);
	void SetScale(glm::vec3 Scale);
	void SetRotate(float Rotate);

	void SetTextureCoord(TEXTURE_MAPPING_TYPE mapping_type,bool is_position);
	void SetNeedCalculateUV(bool tf);

	glm::vec3 GetTranslate();
	std::vector<glm::vec3>& GetVertices();
	std::vector<glm::vec3>& GetVertexNormalLine();
	std::vector<glm::vec3>& GetVertexNormal();
	std::vector<glm::vec3>& GetFaceNormalLine();
	std::vector<glm::vec3>& GetFaceNormal();
	std::vector<glm::vec2>& GetTextureCoord();

	glm::mat4x4 GetTRSMatrix();

	std::vector<unsigned int>& GetIndices();
    int GetVerticesSize() const;
	int GetVertexNormalLineSize() const;
	int GetVertexNormalSize() const;
	int GetFaceNormalLineSize() const;
	int GetFaceNormalSize() const;
	int GetTextureCoordSize() const;
    RENDER_TYPE GetRenderType() const;
	bool GetNeedCalculateUV() const;
private:
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Vertex_Normal_Line;
	std::vector<glm::vec3> Vertex_Normal;
	std::vector<glm::vec3> Face_Normal_Line;
	std::vector<glm::vec3> Face_Normal;
	std::vector<glm::vec2> Texture_Coord;

	glm::vec3 translate;
	glm::vec3 scale;
	float rotate;

	std::vector<unsigned int> face_index;
	RENDER_TYPE RenderType;
	bool need_calculate_uv=false;
};