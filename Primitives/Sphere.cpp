#include "renderer/opengl/Primitives/Sphere.hpp"

//Primitive::PrimitiveType Sphere::type = Primitive::PrimitiveType::Sphere;

Sphere::Sphere() 
	: _radius{1.}
{
}

Sphere::Sphere(float radius)
	: _radius {radius}
{
}

Sphere::Sphere(float radius, glm::vec3 color)
	: _radius{ radius }
{
	_color = color;
}


Sphere::Sphere(const glm::vec3 translation, float radius)
	: Primitive{translation}, _radius{radius}
{
}

Sphere::Sphere(const glm::vec3 translation, const glm::vec3 color, float radius)
	: Primitive{translation, color}, _radius{radius}
{
}

Sphere::Sphere(const glm::mat4 transform, float radius)
	: Primitive{transform}, _radius{radius}
{
}


Sphere::Sphere(const glm::mat4 transform, const glm::vec3 color, float radius)
	: Primitive{transform, color}, _radius{radius}
{
}

Sphere::Sphere(const Sphere& sphere) :
	Primitive{ sphere },
	_radius{ sphere._radius}
{
}

std::vector<uint8_t> Sphere::rawData() const
{
	std::vector<uint8_t> rawData = Primitive::rawData();

	std::vector<uint8_t> sphereData(sizeof(float));
	memcpy(sphereData.data(), &_radius, sizeof(float));

	rawData.insert(rawData.end(), sphereData.begin(), sphereData.end());

	return rawData;
}

bool Sphere::modifySelectedPrimitiveUI(std::string primitiveName)
{
	bool modified = false;

	// basic transform and color modification via UI
	modified = Primitive::modifySelectedPrimitiveUI(primitiveName);

	// special sphere modification via UI
	 
	// draggable widget for radius
	if (ImGui::DragFloat((primitiveName + " radius").c_str(), &_radius, 0.01f, 0.1f))
	{
		if (_radius < 0.001f)
			_radius = 0.001f;

		modified = true;
	}

	return modified;
}