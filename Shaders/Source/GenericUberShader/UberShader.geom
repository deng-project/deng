#version 450
#extension GL_ARB_separate_shader_objects : enable

// NOTE: This geometry shader generates vertex normals from scratch or from vertex tangents
// Avoid using meshes without vertex normals if smooth shading is important

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

#if defined(UV_COORDINATES) && !defined(OUTPUT_UV_COORDINATES)
#define OUTPUT_UV_COORDINATES UV_COORDINATES
#endif

#if defined(COLOR_MULTIPLIERS) && !defined(OUTPUT_COLOR_MULTIPLIERS)
#define OUTPUT_COLOR_MULTIPLIERS COLOR_MULTIPLIERS
#endif

in Vertex {
	layout(location = 0) vec3 vPosition;
#if defined(UV_COORDINATES) && defined(UV_ATTRIB_COUNT)
	layout(location = UV_COORDINATES) vec2 vUVCoordinates[UV_ATTRIB_COUNT];
#endif

#if defined(COLOR_MULTIPLIERS) && defined(COLOR_MULTIPLIER_ATTRIB_COUNT)
	layout(location = COLOR_MULTIPLIERS) vec4 vColorMultipliers[COLOR_MULTIPLIER_ATTRIB_COUNT];
#endif
} inVertices[];

out Vertex {
	layout(location = 0) vec3 vPosition;
	layout(location = 1) vec3 vNormal;

#if defined(UV_COORDINATES) && defined(UV_ATTRIB_COUNT)
	layout(location = OUTPUT_UV_COORDINATES) vec2 vUVCoordinates[UV_ATTRIB_COUNT];
#endif

#if defined(COLOR_MULTIPLIERS) && defined(COLOR_MULTIPLIER_ATTRIB_COUNT)
	layout(location = COLOR_MULTIPLIERS) vec4 vColorMultipliers[COLOR_MULTIPLIER_ATTRIB_COUNT];
#endif
} outVertices;

void main() {
	vec3 a = inVertices[1].vPosition - inVertices[0].vPosition;
	vec3 b = inVertices[2].vPosition - inVertices[0].vPosition;

	if (dot(a, b) < 0) {
		a = inVertices[0].vPosition - inVertices[1].vPosition;
		b = inVertices[2].vPosition - inVertices[1].vPosition;

		if (dot(a, b) < 0) {
			a = inVertices[1].vPosition - inVertices[2].vPosition;
			b = inVertices[0].vPosition - inVertices[2].vPosition;
		}
	}

	vec3 vNormal = cross(a, b);
	vNormal = normalize(vNormal);

	for (int i = 0; i < inVertices.length(); i++) {
		outVertices.vPosition = inVertices[i].vInputPosition;
		outVertices.vNormal = vNormal;
#if defined(UV_COORDINATES) && defined(UV_ATTRIB_COUNT)
		for (int j = 0; j < UV_ATTRIB_COUNT; j++) {
			outVertices.vUVCoordinates[j] = inVertices[i].vUVCoordinates[j];
		}
#endif

#if defined(COLOR_MULTIPLIERS) && defined(COLOR_MULTIPLIER_ATTRIB_COUNT)
		for (int j = 0; j < COLOR_MULTIPLIER_ATTRIB_COUNT; j++) {
			outVertices.vColorMultipliers[j] = inVertices[i].vColorMultipliers[j];
		}
#endif
		EmitVertex();
	}
}