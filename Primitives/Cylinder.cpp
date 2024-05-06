#include "renderer/opengl/Primitives/Cylinder.hpp"

Cylinder::Cylinder() :
	Primitive(),
	_height(1.),
	_radius(1.)
{
}

Cylinder::Cylinder(const float height, const float radius) :
	_height{height},
	_radius{radius}
{
}

Cylinder::Cylinder(const float height, const float radius, glm::vec3 color) :
	_height{height},
	_radius{radius}
{
	_color = color;
}

Cylinder::Cylinder(const glm::vec3 translation, const float height, const float radius) :
	Primitive(translation),
	_height{height},
	_radius{radius}
{
}

Cylinder::Cylinder(const glm::vec3 translation, const glm::vec3 color, const float height, const float radius) :
	Primitive(translation, color),
	_height{height},
	_radius{radius}
{
}

Cylinder::Cylinder(const glm::mat4 transform, const float height, const float radius) :
	Primitive(transform),
	_height{height},
	_radius{radius}
{
}


Cylinder::Cylinder(const glm::mat4 transform, const glm::vec3 color, const float height, const float radius) :
	Primitive(transform, color),
	_height{height},
	_radius{radius}
{
}

Cylinder::Cylinder(const Cylinder& cylinder) :
	Primitive(cylinder),
	_height{cylinder._height},
	_radius{cylinder._radius}
{
}

std::vector<uint8_t> Cylinder::rawData() const
{
	std::vector<uint8_t> rawData = Primitive::rawData();

	std::vector<uint8_t> cylinderData(5 * sizeof(float)); // some strange sizeof() values for memory alignment on GPU side
	memcpy(cylinderData.data(), &_height, sizeof(float));
	memcpy(cylinderData.data() + sizeof(float), &_radius, sizeof(float));
	rawData.insert(rawData.end(), cylinderData.begin(), cylinderData.end());

	return rawData;
}

bool Cylinder::modifySelectedPrimitiveUI(std::string primitiveName)
{
	bool modified = false;

	// basic transform and color modification via UI
	modified = Primitive::modifySelectedPrimitiveUI(primitiveName);

	// special cylinder modification via UI

	// draggable widget for radius
	if (ImGui::DragFloat((primitiveName + " radius").c_str(), &_radius, 0.01f, 0.1f))
	{
		if (_radius < 0.001f)
			_radius = 0.001f;

		modified = true;
	}

	// draggable widget for height
	if (ImGui::DragFloat((primitiveName + " height").c_str(), &_height, 0.01f, 0.1f))
	{
		if (_height < 0.001f)
			_height = 0.001f;

		modified = true;
	}

	return modified;
}