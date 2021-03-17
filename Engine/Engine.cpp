/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.cpp
Purpose: Implementation of Engine class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include "Engine.h"
#include "Graphic.h"
#include "Mesh.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw_gl3.h"

Engine::Engine() :Is_Running(true), obj_file_num(0), window(nullptr),obj_file_list(),graphic(nullptr)
{
}

Engine::~Engine()
{
}

void Engine::MainLoop()
{
	Mesh center_object;
	center_object.Load_Objfile("sphere");
	center_object.SetTranslate(glm::vec3(0, 0, 0));
	center_object.SetScale(glm::vec3{ 0.5f, 0.5f, 0.5f });

	Mesh rotate_line_object;
	rotate_line_object.MakeCircle();
	Mesh rotate_object[16];
	float angle_diff = glm::pi<float>() / 16.f*2.f;
	for (int i = 0; i < 16; ++i)
	{
		//rotate_object[i].Load_Objfile("sphere");
		rotate_object[i].MakeSphere(1.0f, 20);
		rotate_object[i].SetTranslate(glm::vec3(sin(angle_diff*i) * 0.6f, 0, cos(angle_diff * i) * 0.6f));
		rotate_object[i].SetScale(glm::vec3(0.045f, 0.045f, 0.045f));
	}
	glm::vec3 scale_value = { 0.6f ,0.6f,0.6f};
	rotate_line_object.SetTranslate(glm::vec3(0.f, 0, 0));
	rotate_line_object.SetScale(scale_value);

	//assignment1 value
	int previous_obj_num = 0;
	bool draw_vertex_normal = false;
	bool draw_face_normal = false;
	
	//assignment2 value
	bool is_phong_light = false;
	bool is_phong_shading = true;
	bool is_blinn_shading = false;
	SHADER_PROGRAM_TYPE current_program = SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER;

	glm::vec3 all_light_color(1.f);
	bool is_all_point = true;
	bool is_all_direction = false;
	bool is_all_spot = false;
	bool rotate = true;
	int num_lights = 8;
	graphic->Change_Lights_Number(num_lights);
	float dt=0;

	//assignment3 value
	Mesh skybox[6];
	skybox->MakeSkybox();


	while(Is_Running)
	{
		if (Input::Is_Triggered(GLFW_KEY_ESCAPE))
			break;

		graphic->Clear_Buffer();
		glfwPollEvents();
		glfwMakeContextCurrent(window);
		
		if(rotate)
			dt += 0.016f;
		//graphic->Draw_Lines(rotate_line_object);

		graphic->Draw_Skybox_to_FBO(skybox);
		
		for (int i = 0; i < num_lights; ++i)
		{
			rotate_object[i].SetTranslate(glm::vec3(sin(dt + angle_diff * i) * 0.6f, 0, cos(dt + angle_diff * i) * 0.6f));
			graphic->Setting_LightPosDir(i, glm::vec3(sin(dt + angle_diff * i) * 0.6f, 0, cos(dt + angle_diff * i) * 0.6f));
			graphic->Draw_basic_shape_to_FBO(rotate_object[i]);
			graphic->Draw_Basic_Face(rotate_object[i],i);
		}

		graphic->Draw_Face(center_object,current_program);

		ImGui_ImplGlfwGL3_NewFrame();

		ImGui::Begin("Graphics Example");
		if (ImGui::Button("Recompile Shader"))
		{
			graphic->Recompile();
		}
		if (ImGui::CollapsingHeader("Object setting"))
		{
			ImGui::Checkbox("Draw_vertex_normal", &draw_vertex_normal);
			if (draw_vertex_normal)
			{
				//draw vn;
				graphic->Draw_Vertex_Normal(center_object);
			}
			ImGui::Checkbox("Draw_face_normal", &draw_face_normal);
			if (draw_face_normal)
			{
				//draw fn;
				graphic->Draw_Face_Normal(center_object);
			}
			ImGui::NewLine();

			ImGui::Text("Obj file load");
			if (ImGui::Combo("", &obj_file_num, vector_getter,
				&obj_file_list, static_cast<int>(obj_file_list.size())))
			{
				if (previous_obj_num != obj_file_num)//if different number is selected
				{
					center_object.Load_Objfile(obj_file_list[obj_file_num]);
				}
				previous_obj_num = obj_file_num;
			}
		}
		if (ImGui::CollapsingHeader("Light setting"))
		{
			ImGui::Checkbox("Rotate Light", &rotate);
			if (ImGui::SliderInt("Number of lights", &num_lights, 1, 16))
			{
				graphic->Change_Lights_Number(num_lights);
			}

			if (ImGui::Checkbox("phong_light", &is_phong_light))
			{
				current_program = SHADER_PROGRAM_TYPE::PHONG_LIGHT_SHADER;
				is_phong_light = true;
				is_phong_shading = false;
				is_blinn_shading = false;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("phong_shading", &is_phong_shading))
			{
				current_program = SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER;
				is_phong_light = false;
				is_phong_shading = true;
				is_blinn_shading = false;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Blinn_shading", &is_blinn_shading))
			{
				current_program = SHADER_PROGRAM_TYPE::BLINN_PHONG_SHADER;
				is_phong_light = false;
				is_phong_shading = false;
				is_blinn_shading = true;
			}


			if (ImGui::Checkbox("Vertex Position", &graphic->is_vertex_pos))
			{
				graphic->is_vertex_pos = true;
				graphic->is_vertex_normal = false;
				center_object.SetNeedCalculateUV(true);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Vertex Normal", &graphic->is_vertex_normal))
			{
				graphic->is_vertex_pos = false;
				graphic->is_vertex_normal = true;
				center_object.SetNeedCalculateUV(true);
			}


			if (ImGui::Checkbox("Cpu uv Calcuation", &graphic->is_cpu_calculate))
			{
				center_object.SetNeedCalculateUV(true);
			}

			if (ImGui::Checkbox("Cube_mapping", &graphic->is_cubemapping))
			{
				graphic->is_cubemapping = true;
				graphic->is_cylindrical = false;
				graphic->is_spherical = false;

				center_object.SetNeedCalculateUV(true);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Cylindrical", &graphic->is_cylindrical))
			{
				graphic->is_cubemapping = false;
				graphic->is_cylindrical = true;
				graphic->is_spherical = false;

				center_object.SetNeedCalculateUV(true);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Spherical", &graphic->is_spherical))
			{
				graphic->is_cubemapping = false;
				graphic->is_cylindrical = false;
				graphic->is_spherical = true;

				center_object.SetNeedCalculateUV(true);
			}
			ImGui::NewLine();
			ImGui::SliderFloat3("K_ambient", &graphic->K_ambient[0], 0, 1.f);


			//Scenario
			ImGui::NewLine();
			if (ImGui::TreeNode("Scenario"))
			{
				if (ImGui::TreeNode("Scenario1"))
				{
					ImGui::ColorEdit3("All Light Color", &all_light_color[0]);

					if (ImGui::Checkbox("All_Point_Light", &is_all_point))
					{
						is_all_point = true;
						is_all_direction = false;
						is_all_spot = false;
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("All_Directional", &is_all_direction))
					{
						is_all_point = false;
						is_all_direction = true;
						is_all_spot = false;
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("All_Spotlight", &is_all_spot))
					{
						is_all_point = false;
						is_all_direction = false;
						is_all_spot = true;
					}

					if (ImGui::Button("Apply"))
					{
						for (int i = 0; i < num_lights; ++i)
						{
							graphic->light[i].diffuse = all_light_color;
							if (is_all_point)
							{
								graphic->light[i].is_point = true;
								graphic->light[i].is_directional = false;
								graphic->light[i].is_spotlight = false;
							}
							else if (is_all_direction)
							{
								graphic->light[i].is_point = false;
								graphic->light[i].is_directional = true;
								graphic->light[i].is_spotlight = false;
							}
							else if (is_all_spot)
							{
								graphic->light[i].is_point = false;
								graphic->light[i].is_directional = false;
								graphic->light[i].is_spotlight = true;
							}
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Scenario2"))
				{
					if (ImGui::Checkbox("All_Point_Light", &is_all_point))
					{
						is_all_point = true;
						is_all_direction = false;
						is_all_spot = false;
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("All_Directional", &is_all_direction))
					{
						is_all_point = false;
						is_all_direction = true;
						is_all_spot = false;
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("All_Spotlight", &is_all_spot))
					{
						is_all_point = false;
						is_all_direction = false;
						is_all_spot = true;
					}

					if (ImGui::Button("Apply"))
					{
						for (int i = 0; i < num_lights; ++i)
						{
							graphic->light[i].diffuse = glm::vec3(rand()%255/255.f, rand() % 255 / 255.f, rand() % 255 / 255.f);
							if (is_all_point)
							{
								graphic->light[i].is_point = true;
								graphic->light[i].is_directional = false;
								graphic->light[i].is_spotlight = false;
							}
							else if (is_all_direction)
							{
								graphic->light[i].is_point = false;
								graphic->light[i].is_directional = true;
								graphic->light[i].is_spotlight = false;
							}
							else if (is_all_spot)
							{
								graphic->light[i].is_point = false;
								graphic->light[i].is_directional = false;
								graphic->light[i].is_spotlight = true;
							}
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Scenario3"))
				{
					if (ImGui::Button("Cool effect apply"))
					{
						center_object.Load_Objfile("sphere");

						num_lights = 16;
						graphic->Change_Lights_Number(num_lights);
						graphic->is_cpu_calculate = false;

						current_program = SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER;
						is_phong_light = false;
						is_phong_shading = true;
						is_blinn_shading = false;

						for (int i = 0; i < num_lights; ++i)
						{
							graphic->light[i].is_directional = false;
							graphic->light[i].is_point = false;
							graphic->light[i].is_spotlight = true;
							graphic->light[i].diffuse = glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f);

							graphic->light[i].cutOff = 30.f;
							graphic->light[i].outerCutOff = 30.f;
							graphic->light[i].falloff = 10.5f;
						}
					}
					if (ImGui::Button("mix effect apply"))
					{
						center_object.Load_Objfile("sphere");

						num_lights = 16;
						graphic->Change_Lights_Number(num_lights);
						graphic->is_cpu_calculate = false;

						current_program = SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER;
						is_phong_light = false;
						is_phong_shading = true;
						is_blinn_shading = false;

						for (int i = 0; i < num_lights; ++i)
						{
							if (i % 4 == 0)
							{
								graphic->light[i].is_directional = false;
								graphic->light[i].is_point = false;
								graphic->light[i].is_spotlight = true;
								graphic->light[i].diffuse = glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f);

								graphic->light[i].cutOff = 30.f;
								graphic->light[i].outerCutOff = 30.f;
								graphic->light[i].falloff = 10.5f;
							}
							else if(i%2==0)
							{
								graphic->light[i].is_directional = true;
								graphic->light[i].is_point = false;
								graphic->light[i].is_spotlight = false;
								graphic->light[i].diffuse = glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f);
							}
							else
							{
								graphic->light[i].is_directional = false;
								graphic->light[i].is_point = true;
								graphic->light[i].is_spotlight = false;
								graphic->light[i].diffuse = glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f);
							}
						}
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}


			ImGui::Begin("Light Setting");
			for (int i = 0; i < num_lights; ++i)
			{
				if (ImGui::Checkbox((std::string("Point")+std::to_string(i)).c_str(), &graphic->light[i].is_point))
				{
					graphic->light[i].is_point = true;
					graphic->light[i].is_directional = false;
					graphic->light[i].is_spotlight = false;
				}
				ImGui::SameLine();
				if (ImGui::Checkbox((std::string("Directional") + std::to_string(i)).c_str(), &graphic->light[i].is_directional))
				{
					graphic->light[i].is_point = false;
					graphic->light[i].is_directional = true;
					graphic->light[i].is_spotlight = false;
				}
				ImGui::SameLine();
				if (ImGui::Checkbox((std::string("Spotlight") + std::to_string(i)).c_str(), &graphic->light[i].is_spotlight))
				{
					graphic->light[i].is_point = false;
					graphic->light[i].is_directional = false;
					graphic->light[i].is_spotlight = true;
				}
				ImGui::ColorEdit3(("Light" + std::to_string(i)).c_str(), &graphic->light[i].diffuse[0]);
				
				if (graphic->light[i].is_spotlight)
				{
					ImGui::DragFloat(("Light" + std::to_string(i) + "Cutoff").c_str(), &graphic->light[i].cutOff, 0.5f, 0.0f, graphic->light[i].outerCutOff);
					ImGui::DragFloat(("Light" + std::to_string(i) + "outerCutoff").c_str(), &graphic->light[i].outerCutOff, 0.5f, graphic->light[i].cutOff, 180.f);
					ImGui::DragFloat(("Light" + std::to_string(i) + "falloff").c_str(), &graphic->light[i].falloff, 0.1f, 1.f, 100.f);
				}
			}
			ImGui::End();
		}
		if (ImGui::CollapsingHeader("Reflection/Refraction setting"))
		{
			if (ImGui::Checkbox("environment mapping", &graphic->is_environment_mapping))
			{
				if (!graphic->is_environment_mapping)
				{
					graphic->is_environment_mapping_with_phong = false;
				};
			}
			if (ImGui::Checkbox("environment mapping with phong", &graphic->is_environment_mapping_with_phong))
			{
				if (graphic->is_environment_mapping_with_phong)
				{
					graphic->is_environment_mapping = true;
				}
			}

			if (ImGui::Checkbox("reflection", &graphic->is_reflection))
			{

			}
			ImGui::Checkbox("refraction", &graphic->is_refraction);

			ImGui::SliderFloat("refraction ratio_r", &graphic->refraction_ratio_r, 0, 100.f);
			ImGui::SliderFloat("refraction ratio_g", &graphic->refraction_ratio_g, 0, 100.f);
			ImGui::SliderFloat("refraction ratio_b", &graphic->refraction_ratio_b, 0, 100.f);
			if (ImGui::Button("Air"))
			{
				graphic->refraction_ratio_r = 1.000293f;
				graphic->refraction_ratio_g = 1.000293f;
				graphic->refraction_ratio_b = 1.000293f;
			}
			if (ImGui::Button("Hydrogen"))
			{
				graphic->refraction_ratio_r = 1.000132f;
				graphic->refraction_ratio_g = 1.000132f;
				graphic->refraction_ratio_b = 1.000132f;
			}
			if (ImGui::Button("Water"))
			{
				graphic->refraction_ratio_r = 1.333f;
				graphic->refraction_ratio_g = 1.333f;
				graphic->refraction_ratio_b = 1.333f;
			}
			if (ImGui::Button("Olive Oil"))
			{
				graphic->refraction_ratio_r = 1.47f;
				graphic->refraction_ratio_g = 1.47f;
				graphic->refraction_ratio_b = 1.47f;
			}
			if (ImGui::Button("Ice"))
			{
				graphic->refraction_ratio_r = 1.31f;
				graphic->refraction_ratio_g = 1.31f;
				graphic->refraction_ratio_b = 1.31f;
			}
			if (ImGui::Button("Quartz"))
			{
				graphic->refraction_ratio_r = 1.46f;
				graphic->refraction_ratio_g = 1.46f;
				graphic->refraction_ratio_b = 1.46f;
			}
			if (ImGui::Button("Diamond"))
			{
				graphic->refraction_ratio_r = 2.42f;
				graphic->refraction_ratio_g = 2.42f;
				graphic->refraction_ratio_b = 2.42f;
			}
			if (ImGui::Button("Acrylic"))
			{
				graphic->refraction_ratio_r = 1.49f;
				graphic->refraction_ratio_g = 1.49f;
				graphic->refraction_ratio_b = 1.49f;
			}
		}
		ImGui::End();

		ImGui::Render();
		graphic->Draw_Skybox(skybox);
		glfwMakeContextCurrent(window);
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(APP->Get_Window());
	}
}

void Engine::Initailize()
{
	Input::Initialize();
	application.Initialize();
	window = application.Get_Window();

	//Gui setup
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	obj_file_list.push_back("4Sphere");
	obj_file_list.push_back("bunny");
	obj_file_list.push_back("bunny_high_poly");
	obj_file_list.push_back("cube");
	obj_file_list.push_back("cube2");
	obj_file_list.push_back("cup");
	obj_file_list.push_back("lucy_princeton");
	obj_file_list.push_back("quad");
	obj_file_list.push_back("rhino");
	obj_file_list.push_back("sphere");
	obj_file_list.push_back("sphere_modified");
	obj_file_list.push_back("starwars1");
	obj_file_list.push_back("triangle");

	graphic = application.Get_Graphic();

	application.Set_Window_Title("CS300 Assignment1");
}

void Engine::Quit()
{
}
