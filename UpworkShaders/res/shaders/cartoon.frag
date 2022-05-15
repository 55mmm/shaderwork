#version 400

in vec2 passTextureCoords;

out vec4 fragmentColor;

uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D celTexture;

uniform vec2 framebufferSize;

void make_kernel(inout vec4 n[9], sampler2D tex, vec2 coord)
{
	float w = framebufferSize.x / 750000.f;
	float h = framebufferSize.y / 750000.f;

	n[0] = texture2D(tex, coord + vec2( -w, -h));
	n[1] = texture2D(tex, coord + vec2(0.0, -h));
	n[2] = texture2D(tex, coord + vec2(  w, -h));
	n[3] = texture2D(tex, coord + vec2( -w, 0.0));
	n[4] = texture2D(tex, coord);
	n[5] = texture2D(tex, coord + vec2(  w, 0.0));
	n[6] = texture2D(tex, coord + vec2( -w, h));
	n[7] = texture2D(tex, coord + vec2(0.0, h));
	n[8] = texture2D(tex, coord + vec2(  w, h));
}

vec3 make_edge(sampler2D tex)
{
	vec4 n[9];
	make_kernel(n, tex, passTextureCoords);
	vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  	vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
	return sobel.xyz;
}

vec3 edge_color(vec3 raw)
{
	float f = clamp(length(raw), 0.0, 2.0) / 2.0;
	if (f < 0.25)
		return vec3(1.0);
	return vec3(1.0 - smoothstep(0.25, 1.0, f));
}

void main()
{
	vec3 edge = edge_color(make_edge(normalMap) + make_edge(depthMap));
	// Outline only
	// fragmentColor = vec4(edge, 1.0);
	// Colored output
	fragmentColor = vec4(edge * texture(celTexture, passTextureCoords).xyz, 1.0);
}