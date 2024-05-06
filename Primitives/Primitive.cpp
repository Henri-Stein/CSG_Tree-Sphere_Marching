#include "renderer/opengl/Primitives/Primitive.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

Primitive::Primitive() :
	_scale{1.},
	_color{1., 1., 1.}
{
}

Primitive::Primitive(const glm::vec3& translation) :
	_translation{translation},
	_scale{1.},
	_color{1., 1., 1.}
{
}

Primitive::Primitive(const glm::vec3& translation, const glm::vec3& color) :
	_translation{translation},
	_scale{1.},
	_color{color}
{
}

Primitive::Primitive(const glm::mat4& transform) :
	_scale{1.},
	_color{1., 1., 1.}
{
	setTransform(transform);
}

Primitive::Primitive(const glm::mat4& transform, const glm::vec3& color):
	_scale{1.},
	_color(color)
{
	setTransform(transform);
}

Primitive::Primitive(const Primitive& primitive) :
	_translation(primitive._translation),
	_scale(primitive._scale),
	_color(primitive._color)
{
}

[[nodiscard]] glm::mat4 Primitive::getTransform() const
{
	const glm::mat4 translationMat = glm::translate(glm::mat4(1.), _translation);
	const glm::mat4 scaleMat = glm::scale(glm::mat4(1.), _scale);

	return translationMat * glm::mat4(_rotation) * scaleMat;
}

void Primitive::setTransform(const glm::mat4& transform)
{
	glm::vec3 skewIgnored; // Need variables for the function signature but will be ignored
	glm::vec4 perspectiveIgnored;
	
	// decompose model matrix and extract translation, rotation and scale vector
	decompose(transform, _scale, _rotation, _translation, skewIgnored, perspectiveIgnored);
}

// return the data of the object as a vector of uint8_t
// Caution: the transform is given as an inversed mat4 (to avoid heavy computation in shader later)
std::vector<uint8_t> Primitive::rawData() const
{
	const glm::mat4 inverseTransformMat = getInverseTransform();

	std::vector<uint8_t> rawData(sizeof(glm::mat4) + sizeof(glm::vec3));
	memcpy(rawData.data(), &inverseTransformMat, sizeof(glm::mat4));
	memcpy(rawData.data() + sizeof(glm::mat4), &_color, sizeof(glm::vec3));

	return rawData;
}

bool Primitive::modifySelectedPrimitiveUI(std::string primitiveName)
{
	bool modified = false;

	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), primitiveName.c_str());
	// draggable widget for translation
	if (ImGui::DragFloat3((primitiveName + " translation").c_str(), &_translation.x))
	{
		modified = true;
	}

	// draggable widget for rotation
	glm::vec3 euler = getEulerAngles();
	if (ImGui::DragFloat3((primitiveName + " rotation").c_str(), &euler.x, 0.5f, -180.f, 180.f))
	{
		setEulerAngles(euler);
		modified = true;
	}

	// draggable widget for scale
	if (ImGui::DragFloat3((primitiveName + " scale").c_str(), &_scale.x, 0.01f, 0.1f, 4))
	{
		if (_scale.x < 0.001f)
			_scale.x = 0.001f;
		if (_scale.y < 0.001f)
			_scale.y = 0.001f;
		if (_scale.z < 0.001f)
			_scale.z = 0.001f;

		modified = true;
	}

	// draggable widget for color
	float color[3] = { _color.x, _color.y, _color.z };
	if (ImGui::ColorEdit3((primitiveName + " color").c_str(), color))
	{
		_color = glm::vec3(color[0], color[1], color[2]);
		_color[0] = glm::clamp(_color[0], 0.f, 255.f);
		_color[1] = glm::clamp(_color[1], 0.f, 255.f);
		_color[2] = glm::clamp(_color[2], 0.f, 255.f);

		modified = true;
	}

	return modified;
}

Primitive::~Primitive() = default;
