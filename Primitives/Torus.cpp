#include "renderer/opengl/Primitives/Torus.hpp"

Torus::Torus() :
	_majorRadius{ 2. },
	_minorRadius{0.5}
{
}



Torus::Torus(float majorRadius, float minorRadius) :
	_majorRadius{ majorRadius },
	_minorRadius{ minorRadius }
{
}


Torus::Torus(float majorRadius, float minorRadius, glm::vec3 color) :
	_majorRadius{ majorRadius },
	_minorRadius{ minorRadius }
{
	_color = color;
}

Torus::Torus(const glm::vec3 translation, float majorRadius, float minorRadius) :
	Primitive{translation},
	_majorRadius{majorRadius},
	_minorRadius{minorRadius}
{
}


Torus::Torus(const glm::vec3 translation, const glm::vec3 color, float majorRadius, float minorRadius) :
	Primitive{translation, color},
	_majorRadius{majorRadius},
	_minorRadius{minorRadius}
{
}

Torus::Torus(const glm::mat4 transform, float majorRadius, float minorRadius) :
	Primitive{transform},
	_majorRadius{majorRadius},
	_minorRadius{minorRadius}
{
}

Torus::Torus(const glm::mat4 transform, const glm::vec3 color, float majorRadius, float minorRadius) :
	Primitive{transform, color},
	_majorRadius{majorRadius},
	_minorRadius{minorRadius}
{
}

Torus::Torus(const Torus& torus) :
	Primitive{torus},
	_majorRadius{torus._majorRadius},
	_minorRadius{torus._minorRadius}
{
}


std::vector<uint8_t> Torus::rawData() const
{
	std::vector<uint8_t> rawData = Primitive::rawData();

	std::vector<uint8_t> torusData(5 * sizeof(float)); // major and minor radius + 3 * sizeof(float) for memory alignment in shader
	memcpy(torusData.data(), &_majorRadius, sizeof(float));
	memcpy(torusData.data() + sizeof(float), &_minorRadius, sizeof(float));

	rawData.insert(rawData.end(), torusData.begin(), torusData.end());

	return rawData;
}

bool Torus::modifySelectedPrimitiveUI(std::string primitiveName)
{
	bool modified = false;

	// basic transform and color modification via UI
	modified = Primitive::modifySelectedPrimitiveUI(primitiveName);

	// special torus modification via UI
	 
	// draggable widget for major radius
	if (ImGui::DragFloat((primitiveName + " major radius").c_str(), &_majorRadius, 0.01f, 0.1f))
	{
		if (_majorRadius < _minorRadius)
			_majorRadius = _minorRadius + 0.001f;
		if (_majorRadius < 0.001f)
			_majorRadius = 0.001f;

		modified = true;
	}

	// draggable widget for minor radius
	if (ImGui::DragFloat((primitiveName + " minor radius").c_str(), &_minorRadius, 0.01f, 0.1f))
	{
		if (_minorRadius > _majorRadius)
			_minorRadius = _majorRadius - 0.001f;
		if (_minorRadius < 0.001f)
			_minorRadius = 0.001f;

		modified = true;
	}

	return modified;
}