#version 410 core

in vec3 vertex_position;
uniform float time;
out vec3 pos;            // <-- pos now declared here

void main() {
	pos = vertex_position; // <-- removed declaration
	pos.y += sin( time );

	gl_Position = vec4( pos, 1.0 );
}