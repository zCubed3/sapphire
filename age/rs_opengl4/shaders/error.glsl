#ifdef VERTEX
layout(location = 0) in vec3 _VERTEX;

void main() {
    gl_Position = AGE_MVP * vec4(_VERTEX, 1);
}
#endif

#ifdef FRAGMENT
out vec4 color;

void main() {
    color = vec4(1, 0, 0, 1);
}
#endif