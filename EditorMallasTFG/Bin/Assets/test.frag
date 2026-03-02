#version 410 core

in vec3 pos;         // <-- match name from vertex shader.
uniform float time;
out vec4 frag_colour;

void main() {
	// float wild = sin( time ) * 0.5 + 0.5;
	// frag_colour.rba = vec3( 1.0 );
	// frag_colour.g = wild;

	frag_colour = vec4( pos, 1.0 );
}