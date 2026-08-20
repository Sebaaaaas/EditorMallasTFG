#pragma once

enum class ProjectionMode {
    Perspective,
    Orthographic
};

enum class RenderMode {
	Solid,
	Wireframe
};

enum class TransformMode {
    Translate,
    Rotate,
    Scale
};

enum class TransformAxis {
    X,
    Y,
    Z,
    All
};