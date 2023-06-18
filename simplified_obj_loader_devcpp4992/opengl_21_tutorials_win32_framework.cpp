#include "opengl_21_tutorials_win32_framework.h"

// ----------------------------------------------------------------------------------------------------------------------------

CBuffer::CBuffer()
{
	SetDefaults();
}

CBuffer::~CBuffer()
{
	Empty();
}

void CBuffer::AddData(void *Data, int DataSize)
{
	int Remaining = BufferSize - Position;

	if(DataSize > Remaining)
	{
		BYTE *OldBuffer = Buffer;
		int OldBufferSize = BufferSize;

		int Needed = DataSize - Remaining;

		BufferSize += Needed > BUFFER_SIZE_INCREMENT ? Needed : BUFFER_SIZE_INCREMENT;

		Buffer = new BYTE[BufferSize];

		memcpy(Buffer, OldBuffer, OldBufferSize);

		delete [] OldBuffer;
	}

	memcpy(Buffer + Position, Data, DataSize);

	Position += DataSize;
}

void CBuffer::Empty()
{
	delete [] Buffer;

	SetDefaults();
}

void *CBuffer::GetData()
{
	return Buffer;
}

int CBuffer::GetDataSize()
{
	return Position;
}

void CBuffer::SetDefaults()
{
	Buffer = NULL;

	BufferSize = 0;
	Position = 0;
}

// ----------------------------------------------------------------------------------------------------------------------------

int gl_max_texture_size = 0, gl_max_texture_max_anisotropy_ext = 0;

// ----------------------------------------------------------------------------------------------------------------------------

CTexture::CTexture()
{
	Texture = 0;
}

CTexture::~CTexture()
{
}

CTexture::operator GLuint ()
{
	return Texture;
}

bool CTexture::LoadTexture2D(char *FileName)
{
	CString DirectoryFileName = ModuleDirectory + "Textures\\" + FileName;

	int Width, Height, BPP;

	FIBITMAP *dib = GetBitmap(DirectoryFileName, Width, Height, BPP);

	if(dib == NULL)
	{
		ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
		return false;
	}

	GLenum Format = 0;

	if(BPP == 32) Format = GL_BGRA;
	if(BPP == 24) Format = GL_BGR;

	if(Format == 0)
	{
		ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileName);
		FreeImage_Unload(dib);
		return false;
	}

	Destroy();

	glGenTextures(1, &Texture);

	glBindTexture(GL_TEXTURE_2D, Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib));

	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(dib);

	return true;
}

bool CTexture::LoadTextureCubeMap(char **FileNames)
{
	int Width, Height, BPP;

	FIBITMAP *dib[6];

	bool Error = false;
	
	for(int i = 0; i < 6; i++)
	{
		CString DirectoryFileName = ModuleDirectory + "Textures\\" + FileNames[i];

		dib[i] = GetBitmap(DirectoryFileName, Width, Height, BPP);

		if(dib[i] == NULL)
		{
			ErrorLog.Append("Error loading texture " + DirectoryFileName + "!\r\n");
			Error = true;
		}
	}

	if(Error)
	{
		for(int i = 0; i < 6; i++)
		{
			FreeImage_Unload(dib[i]);
		}

		return false;
	}

	GLenum Format = 0;
	
	if(BPP == 32) Format = GL_BGRA;
	if(BPP == 24) Format = GL_BGR;

	if(Format == 0)
	{
		ErrorLog.Append("Unsupported texture format (%s)!\r\n", FileNames[5]);

		for(int i = 0; i < 6; i++)
		{
			FreeImage_Unload(dib[i]);
		}

		return false;
	}

	Destroy();

	glGenTextures(1, &Texture);

	glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);

	for(int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, Width, Height, 0, Format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib[i]));
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	for(int i = 0; i < 6; i++)
	{
		FreeImage_Unload(dib[i]);
	}

	return true;
}

void CTexture::Destroy()
{
	glDeleteTextures(1, &Texture);
	Texture = 0;
}

FIBITMAP *CTexture::GetBitmap(char *FileName, int &Width, int &Height, int &BPP)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(FileName);

	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(FileName);
	}

	if(fif == FIF_UNKNOWN)
	{
		return NULL;
	}

	FIBITMAP *dib = NULL;

	if(FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, FileName);
	}

	if(dib != NULL)
	{
		int OriginalWidth = FreeImage_GetWidth(dib);
		int OriginalHeight = FreeImage_GetHeight(dib);

		Width = OriginalWidth;
		Height = OriginalHeight;

		if(Width == 0 || Height == 0)
		{
			FreeImage_Unload(dib);
			return NULL;
		}

		BPP = FreeImage_GetBPP(dib);

		if(Width > gl_max_texture_size) Width = gl_max_texture_size;
		if(Height > gl_max_texture_size) Height = gl_max_texture_size;

		if(!GLEW_ARB_texture_non_power_of_two)
		{
			Width = 1 << (int)floor((log((float)Width) / log(2.0f)) + 0.5f); 
			Height = 1 << (int)floor((log((float)Height) / log(2.0f)) + 0.5f);
		}

		if(Width != OriginalWidth || Height != OriginalHeight)
		{
			FIBITMAP *rdib = FreeImage_Rescale(dib, Width, Height, FILTER_BICUBIC);
			FreeImage_Unload(dib);
			dib = rdib;
		}
	}

	return dib;
}

// ----------------------------------------------------------------------------------------------------------------------------

CShaderProgram::CShaderProgram()
{
	SetDefaults();
}

CShaderProgram::~CShaderProgram()
{
}

CShaderProgram::operator GLuint ()
{
	return Program;
}

bool CShaderProgram::Load(char *VertexShaderFileName, char *FragmentShaderFileName)
{
	bool Error = false;

	Destroy();

	Error |= ((VertexShader = LoadShader(VertexShaderFileName, GL_VERTEX_SHADER)) == 0);
	Error |= ((FragmentShader = LoadShader(FragmentShaderFileName, GL_FRAGMENT_SHADER)) == 0);

	if(Error)
	{
		Destroy();
		return false;
	}

	Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);
	glLinkProgram(Program);

	int LinkStatus;
	glGetProgramiv(Program, GL_LINK_STATUS, &LinkStatus);

	if(LinkStatus == GL_FALSE)
	{
		ErrorLog.Append("Error linking program (%s, %s)!\r\n", VertexShaderFileName, FragmentShaderFileName);

		int InfoLogLength = 0;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetProgramInfoLog(Program, InfoLogLength, &CharsWritten, InfoLog);
			ErrorLog.Append(InfoLog);
			delete [] InfoLog;
		}

		Destroy();

		return false;
	}

	return true;
}

void CShaderProgram::Destroy()
{
	glDetachShader(Program, VertexShader);
	glDetachShader(Program, FragmentShader);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	glDeleteProgram(Program);

	delete [] UniformLocations;
	delete [] AttribLocations;

	SetDefaults();
}

GLuint CShaderProgram::LoadShader(char *FileName, GLenum Type)
{
	CString DirectoryFileName = ModuleDirectory + "Shaders\\" + FileName;

	FILE *File;

	if((File = fopen(DirectoryFileName, "rb")) == NULL)
	{
		ErrorLog.Append("Error loading file " + DirectoryFileName + "!\r\n");
		return 0;
	}

	fseek(File, 0, SEEK_END);
	long Size = ftell(File);
	fseek(File, 0, SEEK_SET);
	char *Source = new char[Size + 1];
	fread(Source, 1, Size, File);
	fclose(File);
	Source[Size] = 0;

	GLuint Shader = glCreateShader(Type);

	glShaderSource(Shader, 1, (const char**)&Source, NULL);
	delete [] Source;
	glCompileShader(Shader);

	int CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);

	if(CompileStatus == GL_FALSE)
	{
		ErrorLog.Append("Error compiling shader %s!\r\n", FileName);

		int InfoLogLength = 0;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	
		if(InfoLogLength > 0)
		{
			char *InfoLog = new char[InfoLogLength];
			int CharsWritten  = 0;
			glGetShaderInfoLog(Shader, InfoLogLength, &CharsWritten, InfoLog);
			ErrorLog.Append(InfoLog);
			delete [] InfoLog;
		}

		glDeleteShader(Shader);

		return 0;
	}

	return Shader;
}

void CShaderProgram::SetDefaults()
{
	VertexShader = 0;
	FragmentShader = 0;

	Program = 0;

	UniformLocations = NULL;
	AttribLocations = NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------

CCamera::CCamera()
{
	ViewMatrix = NULL;
	ViewMatrixInverse = NULL;

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

CCamera::~CCamera()
{
}

void CCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void CCamera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

vec3 CCamera::OnKeys(BYTE Keys, float FrameTime)
{
	float Speed = 5.0f;

	if(Keys & 0x40) Speed *= 2.0f;
	if(Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * FrameTime;

	vec3 Up(0.0f, 1.0f, 0.0f);
	vec3 Right = X;
	vec3 Forward = cross(Up, Right);

	Up *= Distance;
	Right *= Distance;
	Forward *= Distance;

	vec3 Movement;

	if(Keys & 0x01) Movement += Forward;
	if(Keys & 0x02) Movement -= Forward;
	if(Keys & 0x04) Movement -= Right;
	if(Keys & 0x08) Movement += Right;
	if(Keys & 0x10) Movement += Up;
	if(Keys & 0x20) Movement -= Up;

	return Movement;
}

void CCamera::OnMouseMove(int dx, int dy)
{
	float Sensitivity = 0.25f;

	Position -= Reference;

	if(dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if(dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if(Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);

	CalculateViewMatrix();
}

void CCamera::OnMouseWheel(float zDelta)
{
	Position -= Reference;

	if(zDelta < 0 && length(Position) < 500.0f)
	{
		Position += Position * 0.1f;
	}

	if(zDelta > 0 && length(Position) > 0.05f)
	{
		Position -= Position * 0.1f;
	}

	Position += Reference;

	CalculateViewMatrix();
}

void CCamera::SetViewMatrixPointer(float *ViewMatrix, float *ViewMatrixInverse)
{
	this->ViewMatrix = (mat4x4*)ViewMatrix;
	this->ViewMatrixInverse = (mat4x4*)ViewMatrixInverse;

	CalculateViewMatrix();
}

void CCamera::CalculateViewMatrix()
{
	if(ViewMatrix != NULL)
	{
		*ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);

		if(ViewMatrixInverse != NULL)
		{
			*ViewMatrixInverse = inverse(*ViewMatrix);
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------------------

CCamera Camera;

// ----------------------------------------------------------------------------------------------------------------------------

CObject::CObject()
{
	SetDefaults();
}

CObject::~CObject()
{
}

bool CObject::Load(char *ObjFileName)
{
	CString ModuleDirectoryObjFileName = ModuleDirectory + "Models\\" + ObjFileName;

	FILE *File;

	if((File = fopen(ModuleDirectoryObjFileName, "rb")) == NULL)
	{
		ErrorLog.Append("Error loading file " + ModuleDirectoryObjFileName + "!\r\n");
		return false;
	}

	Destroy();

	char *ObjSource;
	long ObjSourceLength;

	fseek(File, 0, SEEK_END);
	ObjSourceLength = ftell(File);
	fseek(File, 0, SEEK_SET);
	ObjSource = new char[ObjSourceLength + 1];
	fread(ObjSource, 1, ObjSourceLength, File);
	ObjSource[ObjSourceLength] = 0;

	fclose(File);

	for(long i = 0; i < ObjSourceLength; i++)
	{
		if(ObjSource[i] == '\r' || ObjSource[i] == '\n') ObjSource[i] = 0;
	}

	float si, ss, x, y, z, r, g, b, s, t;
	int v1, v2, v3, c1, c2, c3, tc1, tc2, tc3, n1, n2, n3;

	char *Line = ObjSource;
	char *End = ObjSource + ObjSourceLength;

	while(Line < End)
	{
		while(Line < End && (*Line == ' ' || *Line == '\t')) Line++;

		if(Line[0] == '#')
		{
			// skip comment line
		}
		else if(sscanf(Line, "v %f %f %f", &x, &y, &z) == 3)
		{
			VerticesCount++;
		}
		else if(sscanf(Line, "c %f %f %f", &r, &g, &b) == 3)
		{
			ColorsCount++;
		}
		else if(sscanf(Line, "tc %f %f", &s, &t) == 2)
		{
			TexCoordsCount++;
		}
		else if(sscanf(Line, "n %f %f %f", &x, &y, &z) == 3)
		{
			NormalsCount++;
		}
		else if(sscanf(Line, "t %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d", &v1, &c1, &tc1, &n1, &v2, &c2, &tc2, &n2, &v3, &c3, &tc3, &n3) == 12)
		{
			if(v1 < 1 || v1 > VerticesCount || v2 < 1 || v2 > VerticesCount || v3 < 1 || v3 > VerticesCount) { ErrorLog.Append("Invalid vertex index!"); Destroy(); return false; }
			if(c1 < 0 || c1 > ColorsCount || c2 < 0 || c2 > ColorsCount || c3 < 0 || c3 > ColorsCount) { ErrorLog.Append("Invalid color index!"); Destroy(); return false; }
			if(tc1 < 0 || tc1 > TexCoordsCount || tc2 < 0 || tc2 > TexCoordsCount || tc3 < 0 || tc3 > TexCoordsCount) { ErrorLog.Append("Invalid texture coordinate index!"); Destroy(); return false; }
			if(n1 < 0 || n1 > NormalsCount || n2 < 0 || n2 > NormalsCount || n3 < 0 || n3 > NormalsCount) { ErrorLog.Append("Invalid normal index!"); Destroy(); return false; }

			TrianglesCount++;
		}

		while(Line < End && *Line != 0) Line++;
		while(Line < End && *Line == 0) Line++;
	}

	if(VerticesCount > 0) Vertices = new vec3[VerticesCount];
	if(ColorsCount > 0) Colors = new vec3[ColorsCount];
	if(TexCoordsCount > 0) TexCoords = new vec2[TexCoordsCount];
	if(NormalsCount > 0) Normals = new vec3[NormalsCount];

	IndexArray = new CIndexArrayElement[TrianglesCount * 3];

	VerticesCount = ColorsCount = TexCoordsCount = NormalsCount = TrianglesCount = 0;

	Line = ObjSource;

	while(Line < End)
	{
		while(Line < End && (*Line == ' ' || *Line == '\t')) Line++;

		if(Line[0] == '#')
		{
			// skip comment line
		}
		else if(Line[0] == 'd' && Line[1] == 'm' && (Line[2] == ' ' || Line[2] == '\t'))
        {
			char *DiffuseMapFileName = Line + 2;

			while(DiffuseMapFileName < End && (*DiffuseMapFileName == ' ' || *DiffuseMapFileName == '\t')) DiffuseMapFileName++;

			char *Character = DiffuseMapFileName;

			while(Character < End && *Character != 0 && *Character != ' ' && *Character != '\t') Character++;
			while(Character < End && *Character != 0) *Character = 0;

			if(!DiffuseMap.LoadTexture2D(DiffuseMapFileName))
			{
				return false;
			}
		}
		else if(Line[0] == 'n' && Line[1] == 'm' && (Line[2] == ' ' || Line[2] == '\t'))
        {
			char *NormalMapFileName = Line + 2;

			while(NormalMapFileName < End && (*NormalMapFileName == ' ' || *NormalMapFileName == '\t')) NormalMapFileName++;

			char *Character = NormalMapFileName;

			while(Character < End && *Character != 0 && *Character != ' ' && *Character != '\t') Character++;
			while(Character < End && *Character != 0) *Character = 0;

			if(!NormalMap.LoadTexture2D(NormalMapFileName))
			{
				return false;
			}
		}
		else if(sscanf(Line, "si %f", &si) == 1)
		{
			SpecularIntensity = si;
		}
		else if(sscanf(Line, "ss %f", &ss) == 1)
		{
			SpecularShininess = ss;
		}
		else if(sscanf(Line, "v %f %f %f", &x, &y, &z) == 3)
		{
			Vertices[VerticesCount++] = vec3(x, y, z);
		}
		else if(sscanf(Line, "c %f %f %f", &r, &g, &b) == 3)
		{
			Colors[ColorsCount++] = vec3(r, g, b);
		}
		else if(sscanf(Line, "tc %f %f", &s, &t) == 2)
		{
			TexCoords[TexCoordsCount++] = vec2(s, t);
		}
		else if(sscanf(Line, "n %f %f %f", &x, &y, &z) == 3)
		{
			Normals[NormalsCount++] = normalize(vec3(x, y, z));
		}
		else if(sscanf(Line, "t %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d", &v1, &c1, &tc1, &n1, &v2, &c2, &tc2, &n2, &v3, &c3, &tc3, &n3) == 12)
		{
			int ia = TrianglesCount * 3, ib = ia + 1, ic = ib + 1;

			IndexArray[ia].v = v1 - 1; IndexArray[ib].v = v2 - 1; IndexArray[ic].v = v3 - 1;
			IndexArray[ia].c = c1 - 1; IndexArray[ib].c = c2 - 1; IndexArray[ic].c = c3 - 1;
			IndexArray[ia].tc = tc1 - 1; IndexArray[ib].tc = tc2 - 1; IndexArray[ic].tc = tc3 - 1;
			IndexArray[ia].n = n1 - 1; IndexArray[ib].n = n2 - 1; IndexArray[ic].n = n3 - 1;

			TrianglesCount++;
		}

		while(Line < End && *Line != 0) Line++;
		while(Line < End && *Line == 0) Line++;
	}

	delete [] ObjSource;

	VertexArray = new CVertexArrayElement[TrianglesCount * 3];
	
	for(int t = 0; t < TrianglesCount; t++)
	{
		int ia = t * 3, ib = ia + 1, ic = ib + 1;

		VertexArray[ia].Vertex = Vertices[IndexArray[ia].v];
		if(IndexArray[ia].c >= 0) VertexArray[ia].Color = Colors[IndexArray[ia].c]; else VertexArray[ia].Color = vec3(1.0f, 1.0f, 1.0f);
		if(IndexArray[ia].tc >= 0) VertexArray[ia].TexCoord = TexCoords[IndexArray[ia].tc];
		if(IndexArray[ia].n >=0) VertexArray[ia].Normal = Normals[IndexArray[ia].n];

		VertexArray[ib].Vertex = Vertices[IndexArray[ib].v];
		if(IndexArray[ib].c >= 0) VertexArray[ib].Color = Colors[IndexArray[ib].c]; else VertexArray[ib].Color = vec3(1.0f, 1.0f, 1.0f);
		if(IndexArray[ib].tc >= 0) VertexArray[ib].TexCoord = TexCoords[IndexArray[ib].tc];
		if(IndexArray[ib].n >= 0) VertexArray[ib].Normal = Normals[IndexArray[ib].n];

		VertexArray[ic].Vertex = Vertices[IndexArray[ic].v];
		if(IndexArray[ic].c >= 0) VertexArray[ic].Color = Colors[IndexArray[ic].c]; else VertexArray[ic].Color = vec3(1.0f, 1.0f, 1.0f);
		if(IndexArray[ic].tc >= 0) VertexArray[ic].TexCoord = TexCoords[IndexArray[ic].tc];
		if(IndexArray[ic].n >= 0) VertexArray[ic].Normal = Normals[IndexArray[ic].n];

		vec3 vdab = VertexArray[ib].Vertex - VertexArray[ia].Vertex;
		vec3 vdac = VertexArray[ic].Vertex - VertexArray[ia].Vertex;

		vec2 tcdab = VertexArray[ib].TexCoord - VertexArray[ia].TexCoord;
		vec2 tcdac = VertexArray[ic].TexCoord - VertexArray[ia].TexCoord;

		float r = 1.0f / (tcdab.x * tcdac.y - tcdab.y * tcdac.x);

		vec3 Normal = normalize(cross(vdab, vdac));
		vec3 Tangent = normalize((vdab * tcdac.y  - vdac * tcdab.y) * r);
		vec3 Bitangent = normalize((vdac * tcdab.x  - vdab * tcdac.x) * r);

		if(VertexArray[ia].Normal.x == 0.0 && VertexArray[ia].Normal.y == 0.0 && VertexArray[ia].Normal.z == 0.0)
		{
			VertexArray[ia].Normal = Normal;
			VertexArray[ia].Tangent = Tangent;
			VertexArray[ia].Bitangent = Bitangent;
		}
		else
		{
			VertexArray[ia].Normal = normalize(VertexArray[ia].Normal);
			VertexArray[ia].Tangent = normalize(Tangent - VertexArray[ia].Normal * dot(VertexArray[ia].Normal, Tangent));
			VertexArray[ia].Bitangent = cross(VertexArray[ia].Normal, VertexArray[ia].Tangent);
        }

		if(VertexArray[ib].Normal.x == 0.0 && VertexArray[ib].Normal.y == 0.0 && VertexArray[ib].Normal.z == 0.0)
		{
			VertexArray[ib].Normal = Normal;
			VertexArray[ib].Tangent = Tangent;
			VertexArray[ib].Bitangent = Bitangent;
		}
		else
		{
			VertexArray[ib].Normal = normalize(VertexArray[ib].Normal);
			VertexArray[ib].Tangent = normalize(Tangent - VertexArray[ib].Normal * dot(VertexArray[ib].Normal, Tangent));
			VertexArray[ib].Bitangent = cross(VertexArray[ib].Normal, VertexArray[ib].Tangent);
        }

		if(VertexArray[ic].Normal.x == 0.0 && VertexArray[ic].Normal.y == 0.0 && VertexArray[ic].Normal.z == 0.0)
		{
			VertexArray[ic].Normal = Normal;
			VertexArray[ic].Tangent = Tangent;
			VertexArray[ic].Bitangent = Bitangent;
		}
		else
		{
			VertexArray[ic].Normal = normalize(VertexArray[ic].Normal);
			VertexArray[ic].Tangent = normalize(Tangent - VertexArray[ic].Normal * dot(VertexArray[ic].Normal, Tangent));
			VertexArray[ic].Bitangent = cross(VertexArray[ic].Normal, VertexArray[ia].Tangent);
        }
	}

	glGenBuffers(1, &VertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, TrianglesCount * 3 * 68, VertexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

/*void CObject::RenderIndexArray()
{
	glBegin(GL_TRIANGLES);

	for(int t = 0; t < TrianglesCount; t++)
	{
		int ia = t * 3, ib = ia + 1, ic = ib + 1;

		if(IndexArray[ia].c >= 0) glColor3fv(&Colors[IndexArray[ia].c]); else glColor3f(1.0f, 1.0f, 1.0f);
		if(IndexArray[ia].tc >= 0) glTexCoord2fv(&TexCoords[IndexArray[ia].tc]); else glTexCoord2f(0.0f, 0.0f);
		if(IndexArray[ia].n >= 0) glNormal3fv(&Normals[IndexArray[ia].n]); else glNormal3f(0.0f, 0.0f, 0.0f);
		glVertex3fv(&Vertices[IndexArray[ia].v]);

		if(IndexArray[ib].c >= 0) glColor3fv(&Colors[IndexArray[ib].c]); else glColor3f(1.0f, 1.0f, 1.0f);
		if(IndexArray[ib].tc >= 0) glTexCoord2fv(&TexCoords[IndexArray[ib].tc]); else glTexCoord2f(0.0f, 0.0f);
		if(IndexArray[ib].n >= 0) glNormal3fv(&Normals[IndexArray[ib].n]); else glNormal3f(0.0f, 0.0f, 0.0f);
		glVertex3fv(&Vertices[IndexArray[ib].v]);

		if(IndexArray[ic].c >= 0) glColor3fv(&Colors[IndexArray[ic].c]); else glColor3f(1.0f, 1.0f, 1.0f);
		if(IndexArray[ic].tc >= 0) glTexCoord2fv(&TexCoords[IndexArray[ic].tc]); else glTexCoord2f(0.0f, 0.0f);
		if(IndexArray[ic].n >= 0) glNormal3fv(&Normals[IndexArray[ic].n]); else glNormal3f(0.0f, 0.0f, 0.0f);
		glVertex3fv(&Vertices[IndexArray[ic].v]);
	}

	glEnd();
}*/

void CObject::RenderVertexArray(int TangentAttribLocation, int BitangentAttribLocation)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 68, (BYTE*)VertexArray + 0);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 68, (BYTE*)VertexArray + 12);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 68, (BYTE*)VertexArray + 24);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 68, (BYTE*)VertexArray + 32);

	if(TangentAttribLocation >= 0 && BitangentAttribLocation >= 0)
	{
		glEnableVertexAttribArray(TangentAttribLocation);
		glVertexAttribPointer(TangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 68, (BYTE*)VertexArray + 44);

		glEnableVertexAttribArray(BitangentAttribLocation);
		glVertexAttribPointer(BitangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 68, (BYTE*)VertexArray + 56);
	}

	glDrawArrays(GL_TRIANGLES, 0, TrianglesCount * 3);

	if(TangentAttribLocation >= 0 && BitangentAttribLocation >= 0)
	{
		glDisableVertexAttribArray(BitangentAttribLocation);
		glDisableVertexAttribArray(TangentAttribLocation);
	}

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void CObject::RenderVertexBufferObject(int TangentAttribLocation, int BitangentAttribLocation)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 68, (void*)0);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 68, (void*)12);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 68, (void*)24);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 68, (void*)32);

	if(TangentAttribLocation >= 0 && BitangentAttribLocation >= 0)
	{
		glEnableVertexAttribArray(TangentAttribLocation);
		glVertexAttribPointer(TangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 68, (void*)44);

		glEnableVertexAttribArray(BitangentAttribLocation);
		glVertexAttribPointer(BitangentAttribLocation, 3, GL_FLOAT, GL_FALSE, 68, (void*)56);
	}

	glDrawArrays(GL_TRIANGLES, 0, TrianglesCount * 3);

	if(TangentAttribLocation >= 0 && BitangentAttribLocation >= 0)
	{
		glDisableVertexAttribArray(BitangentAttribLocation);
		glDisableVertexAttribArray(TangentAttribLocation);
	}

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CObject::RenderEdges()
{
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	for(int t = 0; t < TrianglesCount; t++)
	{
		int ia = t * 3, ib = ia + 1, ic = ib + 1;

		glVertex3fv(&VertexArray[ia].Vertex); glVertex3fv(&VertexArray[ib].Vertex);
		glVertex3fv(&VertexArray[ib].Vertex); glVertex3fv(&VertexArray[ic].Vertex);
		glVertex3fv(&VertexArray[ic].Vertex); glVertex3fv(&VertexArray[ia].Vertex);
	}

	glEnd();
}

void CObject::RenderTBNs()
{
	glBegin(GL_LINES);

	for(int t = 0; t < TrianglesCount; t++)
	{
		int ia = t * 3, ib = ia + 1, ic = ib + 1;

		bool HasPerVertexNormals = false;

		if(VertexArray[ia].Normal.x != VertexArray[ib].Normal.x || VertexArray[ib].Normal.x != VertexArray[ic].Normal.x)
		{
			HasPerVertexNormals = true;
		}
		else if(VertexArray[ia].Normal.y != VertexArray[ib].Normal.y || VertexArray[ib].Normal.y != VertexArray[ic].Normal.y)
		{
			HasPerVertexNormals = true;
		}
		else if(VertexArray[ia].Normal.z != VertexArray[ib].Normal.z || VertexArray[ib].Normal.z != VertexArray[ic].Normal.z)
		{
			HasPerVertexNormals = true;
		}

		vec3 VertexA = *(vec3*)&(ModelMatrix * vec4(VertexArray[ia].Vertex, 1.0f));
		vec3 VertexB = *(vec3*)&(ModelMatrix * vec4(VertexArray[ib].Vertex, 1.0f));
		vec3 VertexC = *(vec3*)&(ModelMatrix * vec4(VertexArray[ic].Vertex, 1.0f));
		
		if(HasPerVertexNormals)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			
			glVertex3fv(&VertexA); glVertex3fv(&(VertexA + normalize(TangentMatrix * VertexArray[ia].Tangent) * 0.125f));
			glVertex3fv(&VertexB); glVertex3fv(&(VertexB + normalize(TangentMatrix * VertexArray[ib].Tangent) * 0.125f));
			glVertex3fv(&VertexC); glVertex3fv(&(VertexC + normalize(TangentMatrix * VertexArray[ic].Tangent) * 0.125f));

			glColor3f(0.0f, 1.0f, 0.0f);

			glVertex3fv(&VertexA); glVertex3fv(&(VertexA + normalize(TangentMatrix * VertexArray[ia].Bitangent) * 0.125f));
			glVertex3fv(&VertexB); glVertex3fv(&(VertexB + normalize(TangentMatrix * VertexArray[ib].Bitangent) * 0.125f));
			glVertex3fv(&VertexC); glVertex3fv(&(VertexC + normalize(TangentMatrix * VertexArray[ic].Bitangent) * 0.125f));

			glColor3f(0.0f, 0.0f, 1.0f);

			glVertex3fv(&VertexA); glVertex3fv(&(VertexA + normalize(NormalMatrix * VertexArray[ia].Normal) * 0.125f));
			glVertex3fv(&VertexB); glVertex3fv(&(VertexB + normalize(NormalMatrix * VertexArray[ib].Normal) * 0.125f));
			glVertex3fv(&VertexC); glVertex3fv(&(VertexC + normalize(NormalMatrix * VertexArray[ic].Normal) * 0.125f));
		}
		else
		{
			vec3 Middle = (VertexA + VertexB + VertexC) / 3.0f;

			glColor3f(1.0f, 0.0f, 0.0f); glVertex3fv(&Middle); glVertex3fv(&(Middle + normalize(TangentMatrix * VertexArray[ia].Tangent) * 0.125f));
			glColor3f(0.0f, 1.0f, 0.0f); glVertex3fv(&Middle); glVertex3fv(&(Middle + normalize(TangentMatrix * VertexArray[ia].Bitangent) * 0.125f));
			glColor3f(0.0f, 0.0f, 1.0f); glVertex3fv(&Middle); glVertex3fv(&(Middle + normalize(NormalMatrix * VertexArray[ia].Normal) * 0.125f));
		}
	}

	glEnd();
}

void CObject::Destroy()
{
	DiffuseMap.Destroy();
	NormalMap.Destroy();

	delete [] Vertices;
	delete [] Colors;
	delete [] TexCoords;
	delete [] Normals;

	delete [] IndexArray;
	delete [] VertexArray;

	glDeleteBuffers(1, &VertexBufferObject);

	SetDefaults();
}

void CObject::SetModelMatrix(const mat4x4 &ModelMatrix)
{
	this->ModelMatrix = ModelMatrix;
	TangentMatrix = mat3x3(ModelMatrix);
	NormalMatrix = transpose(inverse(TangentMatrix));
}

mat4x4& CObject::GetModelMatrix()
{
	return ModelMatrix;
}

mat3x3& CObject::GetNormalMatrix()
{
	return NormalMatrix;
}

GLuint CObject::GetDiffuseMap()
{
	return DiffuseMap;
}

GLuint CObject::GetNormalMap()
{
	return NormalMap;
}

float CObject::GetSpecularIntensity()
{
	return SpecularIntensity;
}

float CObject::GetSpecularShininess()
{
	return SpecularShininess;
}

void CObject::SetDefaults()
{
	ModelMatrix = mat4x4();
	NormalMatrix = mat3x3();

	VerticesCount = ColorsCount = TexCoordsCount = NormalsCount = TrianglesCount = 0;

	SpecularIntensity = SpecularShininess = 0.0f;

	Vertices = NULL;
	Colors = NULL;
	TexCoords = NULL;
	Normals = NULL;

	IndexArray = NULL;

	VertexArray = NULL;
	VertexBufferObject = 0;
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLRenderer::COpenGLRenderer()
{
	Texturing = true;
	NormalMapping = true;
	RenderEdges = false;
	RenderTBNs = false;
	ShowAxisGrid = true;
	Pause = false;

	Camera.SetViewMatrixPointer(&ViewMatrix);
}

COpenGLRenderer::~COpenGLRenderer()
{
}

bool COpenGLRenderer::Init()
{
	bool Error = false;

	ObjectsCount = 3;

	Objects = new CObject[ObjectsCount];

	char *ObjectsFileNames[] = {"cube1.objs", "cube2.objs", "cylinder.objs"};

	for(int i = 0; i < ObjectsCount; i++)
	{
		Error |= !Objects[i].Load(ObjectsFileNames[i]);
	}

	Error |= !Lighting.Load("lighting.vs", "lighting.fs");

	Lighting.AttribLocations = new GLuint[2];
	Lighting.AttribLocations[0] = glGetAttribLocation(Lighting, "att_Tangent");
	Lighting.AttribLocations[1] = glGetAttribLocation(Lighting, "att_Bitangent");

	Lighting.UniformLocations = new GLuint[8];
	Lighting.UniformLocations[0] = glGetUniformLocation(Lighting, "LightPosition");
	Lighting.UniformLocations[1] = glGetUniformLocation(Lighting, "CameraPosition");
	Lighting.UniformLocations[2] = glGetUniformLocation(Lighting, "ModelMatrix");
	Lighting.UniformLocations[3] = glGetUniformLocation(Lighting, "NormalMatrix");
	Lighting.UniformLocations[4] = glGetUniformLocation(Lighting, "Texturing");
	Lighting.UniformLocations[5] = glGetUniformLocation(Lighting, "NormalMapping");
	Lighting.UniformLocations[6] = glGetUniformLocation(Lighting, "SpecularIntensity");
	Lighting.UniformLocations[7] = glGetUniformLocation(Lighting, "SpecularShininess");

	glUseProgram(Lighting);
	glUniform1i(glGetUniformLocation(Lighting, "DiffuseMap"), 0);
	glUniform1i(glGetUniformLocation(Lighting, "NormalMap"), 1);
	glUseProgram(0);

	if(Error)
	{
		return false;
	}

	Camera.Look(vec3(3.0f, 1.75f, 3.0f), vec3(0.0f, 0.0f, 0.0f));

	return true;
}

void COpenGLRenderer::Render(float FrameTime)
{
	static float a = 0.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&ViewMatrix);

	if(ShowAxisGrid)
	{
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor3f(1.0f, 0.0f, 0.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f,1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f,1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f,0.1f,1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f,0.1f,1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f,-0.1f,1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);

		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_LINES);

		float d = 50.0f;

		for(float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i, 0.0f, -d);
			glVertex3f(i, 0.0f, d);
			glVertex3f(-d, 0.0f, i);
			glVertex3f(d, 0.0f, i);
		}

		glEnd();
	}

	glUseProgram(Lighting);

	glUniform3f(Lighting.UniformLocations[0], 0.0f, 1.0f, 0.0f);
	glUniform3fv(Lighting.UniformLocations[1], 1, &Camera.Position);

	for(int i = 0; i < ObjectsCount; i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&ViewMatrix);
		glMultMatrixf(&Objects[i].GetModelMatrix());

		glUniformMatrix4fv(Lighting.UniformLocations[2], 1, GL_FALSE, &Objects[i].GetModelMatrix());
		glUniformMatrix3fv(Lighting.UniformLocations[3], 1, GL_FALSE, &Objects[i].GetNormalMatrix());
		glUniform1i(Lighting.UniformLocations[4], Texturing && Objects[i].GetDiffuseMap() > 0);
		glUniform1i(Lighting.UniformLocations[5], NormalMapping && Objects[i].GetNormalMap() > 0);
		glUniform1f(Lighting.UniformLocations[6], Objects[i].GetSpecularIntensity());
		glUniform1f(Lighting.UniformLocations[7], Objects[i].GetSpecularShininess());

		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, Objects[i].GetDiffuseMap());
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, Objects[i].GetNormalMap());

		Objects[i].RenderVertexBufferObject(Lighting.AttribLocations[0], Lighting.AttribLocations[1]);

		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);

	if(RenderEdges)
	{
		for(int i = 0; i < ObjectsCount; i++)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&ViewMatrix);
			glMultMatrixf(&Objects[i].GetModelMatrix());

			Objects[i].RenderEdges();
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&ViewMatrix);

	if(RenderTBNs)
	{
		for(int i = 0; i < ObjectsCount; i++)
		{
			Objects[i].RenderTBNs();
		}
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if(!Pause)
	{
		Objects[0].SetModelMatrix(translate(-1.0f, 0.0f, 1.0f) * rotate(a, vec3(0.0f, 1.0f, 0.0f)) * rotate(a, vec3(1.0f, 0.0f, 0.0f)));
		Objects[1].SetModelMatrix(translate(1.0f, 0.0f, -1.0f) * rotate(a, vec3(0.0f, 1.0f, 0.0f)) * rotate(a, vec3(1.0f, 0.0f, 0.0f)));
		Objects[2].SetModelMatrix(translate(-1.0f, 0.0f, -1.0f) * rotate(a, vec3(0.0f, 1.0f, 0.0f)) * rotate(a, vec3(1.0f, 0.0f, 0.0f)));

		a += 11.25f * FrameTime;
	}
}

void COpenGLRenderer::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	glViewport(0, 0, Width, Height);

	ProjectionMatrix = perspective(45.0f, (float)Width / (float)Height, 0.125f, 512.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&ProjectionMatrix);
}

void COpenGLRenderer::Destroy()
{
	for(int i = 0; i < ObjectsCount; i++)
	{
		Objects[i].Destroy();
	}

	delete [] Objects;

	Lighting.Destroy();
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLRenderer OpenGLRenderer;

// ----------------------------------------------------------------------------------------------------------------------------

CString ModuleDirectory, ErrorLog;

// ----------------------------------------------------------------------------------------------------------------------------

void GetModuleDirectory()
{
	char *moduledirectory = new char[256];
	GetModuleFileName(GetModuleHandle(NULL), moduledirectory, 256);
	*(strrchr(moduledirectory, '\\') + 1) = 0;
	ModuleDirectory = moduledirectory;
	delete [] moduledirectory;
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLView::COpenGLView()
{
}

COpenGLView::~COpenGLView()
{
}

bool COpenGLView::Init(HINSTANCE hInstance, char *Title, int Width, int Height, int Samples)
{
	this->Title = Title;
	this->Width = Width;
	this->Height = Height;

	WNDCLASSEX WndClassEx;

	memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.hInstance = hInstance;
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.lpszClassName = "Win32OpenGLWindowClass";

	if(RegisterClassEx(&WndClassEx) == 0)
	{
		ErrorLog.Set("RegisterClassEx failed!");
		return false;
	}

	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, Title, Style, 0, 0, Width, Height, NULL, NULL, hInstance, NULL);

	if(hWnd == NULL)
	{
		ErrorLog.Set("CreateWindowEx failed!");
		return false;
	}

	HDC hDC = GetDC(hWnd);

	if(hDC == NULL)
	{
		ErrorLog.Set("GetDC failed!");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat = ChoosePixelFormat(hDC, &pfd);

	if(PixelFormat == 0)
	{
		ErrorLog.Set("ChoosePixelFormat failed!");
		return false;
	}

	static int MSAAPixelFormat = 0;

	if(SetPixelFormat(hDC, MSAAPixelFormat == 0 ? PixelFormat : MSAAPixelFormat, &pfd) == FALSE)
	{
		ErrorLog.Set("SetPixelFormat failed!");
		return false;
	}

	hGLRC = wglCreateContext(hDC);

	if(hGLRC == NULL)
	{
		ErrorLog.Set("wglCreateContext failed!");
		return false;
	}

	if(wglMakeCurrent(hDC, hGLRC) == FALSE)
	{
		ErrorLog.Set("wglMakeCurrent failed!");
		return false;
	}

	if(glewInit() != GLEW_OK)
	{
		ErrorLog.Set("glewInit failed!");
		return false;
	}

	if(!GLEW_VERSION_2_1)
	{
		ErrorLog.Set("OpenGL 2.1 not supported!");
		return false;
	}

	if(MSAAPixelFormat == 0 && Samples > 0)
	{
		if(GLEW_ARB_multisample && WGLEW_ARB_pixel_format)
		{
			while(Samples > 0)
			{
				UINT NumFormats = 0;

				int PFAttribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, Samples,
					0
				};

				if(wglChoosePixelFormatARB(hDC, PFAttribs, NULL, 1, &MSAAPixelFormat, &NumFormats) == TRUE && NumFormats > 0) break;

				Samples--;
			}

			wglDeleteContext(hGLRC);
			DestroyWindow(hWnd);
			UnregisterClass(WndClassEx.lpszClassName, hInstance);

			return Init(hInstance, Title, Width, Height, Samples);
		}
		else
		{
			Samples = 0;
		}
	}

	this->Samples = Samples;

	GetModuleDirectory();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

	if(GLEW_EXT_texture_filter_anisotropic)
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);
	}

	if(WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}

	return OpenGLRenderer.Init();
}

void COpenGLView::Show(bool Maximized)
{
	RECT dRect, wRect, cRect;

	GetWindowRect(GetDesktopWindow(), &dRect);
	GetWindowRect(hWnd, &wRect);
	GetClientRect(hWnd, &cRect);

	wRect.right += Width - cRect.right;
	wRect.bottom += Height - cRect.bottom;
	wRect.right -= wRect.left;
	wRect.bottom -= wRect.top;
	wRect.left = dRect.right / 2 - wRect.right / 2;
	wRect.top = dRect.bottom / 2 - wRect.bottom / 2;

	MoveWindow(hWnd, wRect.left, wRect.top, wRect.right, wRect.bottom, FALSE);

	ShowWindow(hWnd, Maximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL);
}

void COpenGLView::MessageLoop()
{
	MSG Msg;

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

void COpenGLView::Destroy()
{
	if(GLEW_VERSION_2_1)
	{
		OpenGLRenderer.Destroy();
	}

	wglDeleteContext(hGLRC);
	DestroyWindow(hWnd);
}

void COpenGLView::OnKeyDown(UINT Key)
{
	switch(Key)
	{
		case VK_F1:
			OpenGLRenderer.Texturing = !OpenGLRenderer.Texturing;
			break;

		case VK_F2:
			OpenGLRenderer.NormalMapping = !OpenGLRenderer.NormalMapping;
			break;

		case VK_F3:
			OpenGLRenderer.RenderEdges = !OpenGLRenderer.RenderEdges;
			break;

		case VK_F4:
			OpenGLRenderer.RenderTBNs = !OpenGLRenderer.RenderTBNs;
			break;

		case VK_F5:
			OpenGLRenderer.ShowAxisGrid = !OpenGLRenderer.ShowAxisGrid;
			break;

		case VK_SPACE:
			OpenGLRenderer.Pause = !OpenGLRenderer.Pause;
			break;
	}
}

void COpenGLView::OnMouseMove(int X, int Y)
{
	if(GetKeyState(VK_RBUTTON) & 0x80)
	{
		Camera.OnMouseMove(LastX - X, LastY - Y);

		LastX = X;
		LastY = Y;
	}
}

void COpenGLView::OnMouseWheel(short zDelta)
{
	Camera.OnMouseWheel(zDelta);
}

void COpenGLView::OnPaint()
{
	static DWORD LastFPSTime = GetTickCount(), LastFrameTime = LastFPSTime, FPS = 0;

	PAINTSTRUCT ps;

	HDC hDC = BeginPaint(hWnd, &ps);

	DWORD Time = GetTickCount();

	float FrameTime = (Time - LastFrameTime) * 0.001f;

	LastFrameTime = Time;

	if(Time - LastFPSTime > 1000)
	{
		CString Text = Title;

		if(OpenGLRenderer.Text[0] != 0)
		{
			Text.Append(" - " + OpenGLRenderer.Text);
		}

		Text.Append(" - %dx%d", Width, Height);
		Text.Append(", ATF %dx", gl_max_texture_max_anisotropy_ext);
		Text.Append(", MSAA %dx", Samples);
		Text.Append(", FPS: %d", FPS);
		Text.Append(" - %s", glGetString(GL_RENDERER));
		
		SetWindowText(hWnd, Text);

		LastFPSTime = Time;
		FPS = 0;
	}
	else
	{
		FPS++;
	}

	BYTE Keys = 0x00;

	if(GetKeyState('W') & 0x80) Keys |= 0x01;
	if(GetKeyState('S') & 0x80) Keys |= 0x02;
	if(GetKeyState('A') & 0x80) Keys |= 0x04;
	if(GetKeyState('D') & 0x80) Keys |= 0x08;
	if(GetKeyState('R') & 0x80) Keys |= 0x10;
	if(GetKeyState('F') & 0x80) Keys |= 0x20;

	if(GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
	if(GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;

	if(Keys & 0x3F)
	{
		Camera.Move(Camera.OnKeys(Keys, FrameTime));
	}

	OpenGLRenderer.Render(FrameTime);

	SwapBuffers(hDC);

	EndPaint(hWnd, &ps);

	InvalidateRect(hWnd, NULL, FALSE);
}

void COpenGLView::OnRButtonDown(int X, int Y)
{
	LastX = X;
	LastY = Y;
}

void COpenGLView::OnSize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;

	OpenGLRenderer.Resize(Width, Height);
}

// ----------------------------------------------------------------------------------------------------------------------------

COpenGLView OpenGLView;

// ----------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
			OpenGLView.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
			break;

		case 0x020A: // WM_MOUSWHEEL
			OpenGLView.OnMouseWheel(HIWORD(wParam));
			break;

		case WM_KEYDOWN:
			OpenGLView.OnKeyDown((UINT)wParam);
			break;

		case WM_PAINT:
			OpenGLView.OnPaint();
			break;

		case WM_RBUTTONDOWN:
			OpenGLView.OnRButtonDown(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_SIZE:
			OpenGLView.OnSize(LOWORD(lParam), HIWORD(lParam));
			break;

		default:
			return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	return 0;
}

// ----------------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR sCmdLine, int iShow)
{
	char *AppName = "Simplified OBJ loader";

	if(OpenGLView.Init(hInstance, AppName, 800, 600, 4))
	{
		OpenGLView.Show();
		OpenGLView.MessageLoop();
	}
	else
	{
		MessageBox(NULL, ErrorLog, AppName, MB_OK | MB_ICONERROR);
	}

	OpenGLView.Destroy();

	return 0;
}
