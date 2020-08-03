#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "../myGLFW/glExtensions.h"
#include "../3DMath/matrixMath.h"
#include <gl/GL.h>

typedef struct  {
	vec3f ambient;
	vec3f diffuse;
	vec3f specular;
	int emitter;
} phong_material;

typedef struct {
	phong_material material;
	char* materialName;
} mtllib_material;

typedef struct {
	unsigned int numMaterials;
	mtllib_material* materials;
} mtllib;

typedef struct {
	int numLines;
	char** lines;
	int* lengths;
} lines_data;

typedef struct {
	int* indices;
	vec3f* normals;
	vec3f* positions;
	
	unsigned int numTris;
	unsigned int numUniqueIndices;
	
	int* materialBounds;
	unsigned int numMaterials;

	mtllib materials;
} obj_data;

typedef struct {
	unsigned int width;
	unsigned int height;
	char* lpBits;
} image_bit_data;

lines_data get_file_lines(const char* filename);
void free_lines_data(lines_data* ld);

GLuint create_vertex_shader(const char* shaderName);
GLuint create_fragment_shader(const char* shaderName);
GLuint create_program(GLuint* shaders, int numShaders);

mtllib read_mtl_file(const char* filename);
void free_mtllib_data(mtllib* ml);

obj_data read_obj_file(const char* filename, const char* mtlFilename);
void free_obj_data(obj_data* od);

image_bit_data read_png_file(const char* filename);
image_bit_data read_png_file_simple(const char* filename);
void free_image_bit_data(image_bit_data* ibd);

image_bit_data get_image_rect(const image_bit_data src, int x, int y, int cx, int cy);

#endif