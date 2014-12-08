/* Parallax Mapping by Gabriel Fonseca Silva*/
#include "maths_funcs.h"
#include "gl_utils.h"
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GL_LOG_FILE "gl.log"

#include <iostream>
using namespace std;

int g_gl_width = 1024;
int g_gl_height = 768;
GLFWwindow* g_window = NULL;

bool load_texture (const char* file_name, GLuint* tex) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load (file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf (stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
			);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures (1, tex);
	glBindTexture (GL_TEXTURE_2D, *tex);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data );
	glGenerateMipmap (GL_TEXTURE_2D);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}

int main () {

	//GL Log
	assert (restart_gl_log ());
	assert (start_gl ());

	//OpenGL Set Up
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_CULL_FACE);
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);

	//VBOs
	GLfloat points[] = {
		-1.0f, -0.5f,  0.0f,
		0.0f, -0.5f,  0.0f,
		0.0f,  0.5f,  0.0f,
		0.0f,  0.5f,  0.0f,
		-1.0f,  0.5f,  0.0f,
		-1.0f, -0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		1.5f, -0.5f,  0.0f,
		1.5f, 0.5f,  0.0f,
		1.5f, 0.5f,  0.0f,
		0.5f, 0.5f,  0.0f,
		0.5f, -0.5f,  0.0f
	};


	
	GLfloat normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	GLfloat texcoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	GLfloat tangents[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};
	
	//Material set
	GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat mat_difuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mat_shininess = 500.0;

	//Light set
	GLfloat light_position[] = { 0.0f, 0.0f, 2.0f};
	GLfloat light_intensity[] = {1.0f, 1.0f, 1.0f};

	//Parallax Variables
	GLfloat scale_bias[] = { -0.1f, 0.05f};

	//Set the VBOs and the VAO
	GLuint points_vbo;
	glGenBuffers (1, &points_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
	glBufferData (GL_ARRAY_BUFFER, 36 * sizeof (GLfloat), points, GL_STATIC_DRAW);
	GLuint texcoords_vbo;
	glGenBuffers (1, &texcoords_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
	glBufferData (GL_ARRAY_BUFFER, 24 * sizeof (GLfloat), texcoords, GL_STATIC_DRAW);
	GLuint normal_vbo;
	glGenBuffers (1, &normal_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, normal_vbo);
	glBufferData (GL_ARRAY_BUFFER, 36 * sizeof (GLfloat), normals, GL_STATIC_DRAW);
	GLuint tangent_vbo;
	glGenBuffers (1, &tangent_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, tangent_vbo);
	glBufferData (GL_ARRAY_BUFFER, 48 * sizeof (GLfloat), tangents, GL_STATIC_DRAW);
		
	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer (GL_ARRAY_BUFFER, normal_vbo);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer (GL_ARRAY_BUFFER, tangent_vbo);
	glVertexAttribPointer (3, 4, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray (0);
	glEnableVertexAttribArray (1);
	glEnableVertexAttribArray (2);
	glEnableVertexAttribArray (3);
	
	//Load the Shadders
	//GLuint shader_programme = create_programme_from_files ("NormalMappingVertexShader.glsl", "NormalMappingFragmentShader.glsl");
	GLuint shader_programme = create_programme_from_files ("ParallaxVertexShader.glsl", "ParallaxFragmentShader.glsl");
	

	//Set Up the camera
	#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 
	float near = 0.1f;
	float far = 100.0f;
	float fov = 67.0f * ONE_DEG_IN_RAD;
	float aspect = (float)g_gl_width / (float)g_gl_height;

	float range = tan (fov * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);
	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};

	float cam_speed = 1.0f;
	float cam_yaw_speed = 10.0f;
	float cam_pos[] = {0.0f, 0.0f, 2.0f};
	float cam_yaw = 0.0f;
	mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw);
	mat4 view_mat = R * T;

	//Normal matrix set
	mat4 normal_mat;
	normal_mat = transpose(inverse(view_mat));

	//Set Unifor variables on the GLSL
	//Matrix
	int model_mat_location = glGetUniformLocation (shader_programme, "model");
	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	int normal_mat_location = glGetUniformLocation (shader_programme, "normalMatrix");
	//Light
	int light_position_location = glGetUniformLocation (shader_programme, "lightPosition");
	int light_intensity_location = glGetUniformLocation (shader_programme, "lightIntensity");
	//Parallax Info
	int scale_bias_location = glGetUniformLocation (shader_programme, "scaleBias");
	//Material
	int material_ambient_location = glGetUniformLocation (shader_programme, "materialAmbient");
	int material_difuse_location = glGetUniformLocation (shader_programme, "materialDiffuse");
	int material_specular_location = glGetUniformLocation (shader_programme, "materialSpecular");
	int material_shininess_location = glGetUniformLocation (shader_programme, "materialShininess");
	//Textures
	int tex_a_location = glGetUniformLocation (shader_programme, "textureDiffuse");
	//assert (tex_a_location > -1);
	int tex_b_location = glGetUniformLocation (shader_programme, "textureNormal");
	//assert (tex_b_location > -1);
	int tex_c_location = glGetUniformLocation (shader_programme, "textureHeight");
	//assert (tex_c_location > -1);

	glUseProgram (shader_programme);
	//Matrix
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, identity_mat4 ().m);
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat);
	glUniformMatrix4fv (normal_mat_location, 1, GL_FALSE, normal_mat.m);
	//Light
	glUniform3fv(light_position_location, 1, light_position);
	glUniform3fv(light_intensity_location, 1, light_intensity);
	//Parallax Info
	glUniform2fv(scale_bias_location, 1 , scale_bias);
	//Material
	glUniform4fv(material_ambient_location, 1, mat_ambient);
	glUniform4fv(material_difuse_location, 1, mat_difuse);
	glUniform4fv(material_specular_location, 1, mat_specular);
	glUniform1f(material_shininess_location, mat_shininess);
	//Textures
	glUniform1i (tex_a_location, 0);
	glUniform1i (tex_b_location, 1);
	glUniform1i (tex_c_location, 2);

	//Load textures
	GLuint tex_a, tex_b, tex_c;
	glActiveTexture (GL_TEXTURE0);
	assert (load_texture ("Tex_Diffuse.jpg", &tex_a));
	glBindTexture (GL_TEXTURE_2D, tex_a);
	glActiveTexture (GL_TEXTURE1);
	assert (load_texture ("Tex_Normal.jpg", &tex_b));
	glBindTexture (GL_TEXTURE_2D, tex_b);
	glActiveTexture (GL_TEXTURE2);
	assert (load_texture ("Tex_Height.jpg", &tex_c));
	glBindTexture (GL_TEXTURE_2D, tex_c);

	while (!glfwWindowShouldClose (g_window)) {
		static double previous_seconds = glfwGetTime ();
		double current_seconds = glfwGetTime ();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;
	
		_update_fps_counter (g_window);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport (0, 0, g_gl_width, g_gl_height);
		
		glUseProgram (shader_programme);
		glBindVertexArray (vao);
		glDrawArrays (GL_TRIANGLES, 0, 12);
		glfwPollEvents ();
		
		// control keys
		bool cam_moved = false;
		if (glfwGetKey (g_window, GLFW_KEY_A)) {
			cam_pos[0] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_D)) {
			cam_pos[0] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_PAGE_UP)) {
			cam_pos[1] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_PAGE_DOWN)) {
			cam_pos[1] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_W)) {
			cam_pos[2] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_S)) {
			cam_pos[2] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_LEFT)) {
			cam_yaw += cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey (g_window, GLFW_KEY_RIGHT)) {
			cam_yaw -= cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (cam_moved) {
			mat4 T = translate (identity_mat4 (), vec3 (-cam_pos[0], -cam_pos[1], -cam_pos[2]));
			mat4 R = rotate_y_deg (identity_mat4 (), -cam_yaw);
			mat4 view_mat = R * T;
			glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
		}


		if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose (g_window, 1);
		}

		// parallax parameter control
		if (glfwGetKey (g_window, GLFW_KEY_R)) {
			scale_bias[0] = -0.1f;
			scale_bias[1] = 0.05f;
			glUniform2fv(scale_bias_location, 1, scale_bias);
		}
		if (glfwGetKey (g_window, GLFW_KEY_T)) {
			scale_bias[0] += 0.01f;
			glUniform2fv(scale_bias_location, 1, scale_bias);
		}
		if (glfwGetKey (g_window, GLFW_KEY_G)) {
			scale_bias[0] -= 0.01f;
			glUniform2fv(scale_bias_location, 1, scale_bias);
		}
		if (glfwGetKey (g_window, GLFW_KEY_Y)) {
			scale_bias[1] += 0.01f;
			glUniform2fv(scale_bias_location, 1, scale_bias);
		}
		if (glfwGetKey (g_window, GLFW_KEY_H)) {
			scale_bias[1] -= 0.01f;
			glUniform2fv(scale_bias_location, 1, scale_bias);
		}
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}
