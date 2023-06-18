#version 120

uniform mat4x4 ModelMatrix;
uniform mat3x3 NormalMatrix;
uniform bool Texturing, NormalMapping;

attribute vec3 att_Tangent, att_Bitangent;

varying vec3 var_Position, var_Normal, var_Tangent, var_Bitangent;

void main()
{
	gl_FrontColor = gl_Color;

	if(Texturing || NormalMapping)
	{
		gl_TexCoord[0] = gl_MultiTexCoord0;
	}

	var_Position = (ModelMatrix * gl_Vertex).xyz;

	if(NormalMapping)
	{
		var_Normal = gl_Normal;
		var_Tangent = att_Tangent;
		var_Bitangent = att_Bitangent;
	}
	else
	{
		var_Normal = NormalMatrix * gl_Normal;
	}

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
