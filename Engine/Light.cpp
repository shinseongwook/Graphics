/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.cpp
Purpose: Implementation of Light class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 11/10/2019
End Header --------------------------------------------------------*/
#include "Light.h"
#include "Graphic.h"

Light::Light()
{
}

Light::Light(int num, Graphic* ptr)
{
	light_num = num;
	if (light_num == 0)
	{
		is_active = true;
	}
	graphic = ptr;
}

void Light::WritetoShader()
{
	for (int i = 1; i < static_cast<int>(SHADER_PROGRAM_TYPE::MAX_SHADER); ++i)
	{
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].is_active", is_active);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].is_point", is_point);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].is_directional", is_directional);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].is_spotlight", is_spotlight);

		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].position", pos);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].direction", direction);

		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].cutOff", glm::cos(glm::radians(cutOff)));
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].outerCutOff", glm::cos(glm::radians(outerCutOff)));
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].falloff", falloff);

		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].ambient", ambient);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].diffuse", diffuse);
		graphic->WritetoAllShader("Lights[" + std::to_string(light_num) + "].specular", specular);
	}
}
