// ----------------------------------------------------------------------------------------------------------------------------
//
// Version 2.02
//
// ----------------------------------------------------------------------------------------------------------------------------

#include <windows.h>

#include "glmath.h"
#include "string.h"

#include <gl/glew.h> // http://glew.sourceforge.net/
#include <gl/wglew.h>

#include <FreeImage.h> // http://freeimage.sourceforge.net/

// ----------------------------------------------------------------------------------------------------------------------------

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")

// ----------------------------------------------------------------------------------------------------------------------------

extern CString ModuleDirectory, ErrorLog;

// ----------------------------------------------------------------------------------------------------------------------------

#define BUFFER_SIZE_INCREMENT 1048576

// ----------------------------------------------------------------------------------------------------------------------------

class CBuffer
{
private:
	BYTE *Buffer;
	int BufferSize, Position;

public:
	CBuffer();
	~CBuffer();

	void AddData(void *Data, int DataSize);
	void Empty();
	void *GetData();
	int GetDataSize();

private:
	void SetDefaults();
};

// ----------------------------------------------------------------------------------------------------------------------------

extern int gl_max_texture_size, gl_max_texture_max_anisotropy_ext;

// ----------------------------------------------------------------------------------------------------------------------------

class CTexture
{
protected:
	GLuint Texture;

public:
	CTexture();
	~CTexture();

	operator GLuint ();

	bool LoadTexture2D(char *FileName);
	bool LoadTextureCubeMap(char **FileNames);
	void Destroy();

protected:
	FIBITMAP *CTexture::GetBitmap(char *FileName, int &Width, int &Height, int &BPP);
};

// ----------------------------------------------------------------------------------------------------------------------------

class CShaderProgram
{
protected:
	GLuint VertexShader, FragmentShader, Program;

public:
	GLuint *UniformLocations, *AttribLocations;

public:
	CShaderProgram();
	~CShaderProgram();

	operator GLuint ();

	bool Load(char *VertexShaderFileName, char *FragmentShaderFileName);
	void Destroy();

protected:
	GLuint LoadShader(char *FileName, GLenum Type);
	void SetDefaults();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CCamera
{
protected:
	mat4x4 *ViewMatrix, *ViewMatrixInverse;

public:
	vec3 X, Y, Z, Position, Reference;

	CCamera();
	~CCamera();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void Move(const vec3 &Movement);
	vec3 OnKeys(BYTE Keys, float FrameTime);
	void OnMouseMove(int dx, int dy);
	void OnMouseWheel(float zDelta);
	void SetViewMatrixPointer(float *ViewMatrix, float *ViewMatrixInverse = NULL);

private:
	void CalculateViewMatrix();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CIndexArrayElement
{
public:
	int v, c, tc, n;
};

// ----------------------------------------------------------------------------------------------------------------------------

class CVertexArrayElement
{
public:
	vec3 Vertex;
	vec3 Color;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
};

// ----------------------------------------------------------------------------------------------------------------------------

class CObject
{
protected:
	mat4x4 ModelMatrix;
	mat3x3 TangentMatrix, NormalMatrix;

protected:
	int VerticesCount, ColorsCount, TexCoordsCount, NormalsCount;

protected:
	int TrianglesCount;

protected:
	CTexture DiffuseMap, NormalMap;

protected:
	float SpecularIntensity, SpecularShininess;

protected:
	vec3 *Vertices;
	vec3 *Colors;
	vec2 *TexCoords;
	vec3 *Normals;

protected:
	CIndexArrayElement *IndexArray;

protected:
	CVertexArrayElement *VertexArray;
	GLuint VertexBufferObject;

public:
	CObject();
	~CObject();

	bool Load(char *ObjFileName);
	void RenderVertexArray(int TangentAttribLocation = -1, int BitangentAttribLocation = -1);
	void RenderVertexBufferObject(int TangentAttribLocation = -1, int BitangentAttribLocation = -1);
	void RenderEdges();
	void RenderTBNs();
	void Destroy();

	void SetModelMatrix(const mat4x4 &ModelMatrix);
	mat4x4& GetModelMatrix();
	mat3x3& GetNormalMatrix();
	GLuint GetDiffuseMap();
	GLuint GetNormalMap();
	float GetSpecularIntensity();
	float GetSpecularShininess();

protected:
	void SetDefaults();
};

// ----------------------------------------------------------------------------------------------------------------------------

class COpenGLRenderer
{
protected:
	int Width, Height;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

protected:
	CObject *Objects;
	int ObjectsCount;
	CShaderProgram Lighting;

public:
	bool Texturing, NormalMapping, RenderEdges, RenderTBNs, ShowAxisGrid, Pause;

public:
	CString Text;

public:
	COpenGLRenderer();
	~COpenGLRenderer();

	bool Init();
	void Render(float FrameTime);
	void Resize(int Width, int Height);
	void Destroy();
};

// ----------------------------------------------------------------------------------------------------------------------------

class COpenGLView
{
protected:
	char *Title;
	int Width, Height, Samples;
	HWND hWnd;
	HGLRC hGLRC;

protected:
	int LastX, LastY;

public:
	COpenGLView();
	~COpenGLView();

	bool Init(HINSTANCE hInstance, char *Title, int Width, int Height, int Samples);
	void Show(bool Maximized = false);
	void MessageLoop();
	void Destroy();

	void OnKeyDown(UINT Key);
	void OnMouseMove(int X, int Y);
	void OnMouseWheel(short zDelta);
	void OnPaint();
	void OnRButtonDown(int X, int Y);
	void OnSize(int Width, int Height);
};

// ----------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

// ----------------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int iShow);
