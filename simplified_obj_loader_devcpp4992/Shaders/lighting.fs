#version 120

uniform sampler2D DiffuseMap, NormalMap;
uniform mat3x3 NormalMatrix;
uniform vec3 LightPosition, CameraPosition;
uniform float SpecularIntensity, SpecularShininess;
uniform bool Texturing, NormalMapping;

varying vec3 var_Position, var_Normal, var_Tangent, var_Bitangent;

void main()
{
	gl_FragColor = gl_Color;

	if(Texturing)
	{
		gl_FragColor.rgb *= texture2D(DiffuseMap, gl_TexCoord[0].st).rgb;
	}

	vec3 Normal = normalize(var_Normal);

	if(NormalMapping)
	{
		vec3 Tangent = normalize(var_Tangent);
		vec3 Bitangent = normalize(var_Bitangent);

		mat3x3 TBN = mat3x3(Tangent, Bitangent, Normal);

		Normal = normalize(NormalMatrix * (TBN * (texture2D(NormalMap, gl_TexCoord[0].st).rgb * 2.0 - 1.0)));
	}

	vec3 LightDirection = normalize(LightPosition - var_Position);

	float NdotLD = max(dot(Normal, LightDirection), 0.0);

	gl_FragColor.rgb *= 0.25 + 0.75 * NdotLD;

	if(SpecularIntensity > 0.0 && SpecularShininess >= 0.0)
	{
		vec3 CameraDirection = normalize(CameraPosition - var_Position);
		vec3 LightDirectionReflected = reflect(-LightDirection, Normal);

		float CDdotLDR = max(dot(CameraDirection, LightDirectionReflected), 0.0);

		gl_FragColor.rgb += SpecularIntensity * pow(CDdotLDR, SpecularShininess);
	}
}
