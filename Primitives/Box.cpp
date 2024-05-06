#include "renderer/opengl/Primitives/Box.hpp"

Box::Box() : 
	_size{1.}
{
}


Box::Box(const float sizeX, const float sizeY, const float sizeZ) :
	_size{sizeX, sizeY, sizeZ}
{
}

Box::Box(const glm::vec3 size) :
	_size{size}
{
}

Box::Box(const float sizeX, const float sizeY, const float sizeZ, glm::vec3 color) :
	_size{sizeX, sizeY, sizeZ}
{
	_color = color;
}

Box::Box(const glm::vec3 size, glm::vec3 color) :
	_size{size}
{
	_color = color;
}


Box::Box(const glm::vec3 translation, const glm::vec3 color, glm::vec3 size) :
	Primitive{translation, color},
	_size{size}
{
}

Box::Box(const glm::mat4 transform, const glm::vec3 size) :
	Primitive{transform},
	_size{size}
{
}

Box::Box(const glm::mat4 transform, const glm::vec3 color, const glm::vec3 size) :
	Primitive{transform, color},
	_size{size}
{
}

Box::Box(const Box& box) :
	Primitive{box},
	_size{box._size}
{}


std::vector<uint8_t> Box::rawData() const
{
	std::vector<uint8_t> rawData = Primitive::rawData();

	//std::vector<uint8_t> boxData(sizeof(float) + sizeof(glm::vec3)); // some strange sizeof() values for memory alignment on GPU side
	//memcpy(boxData.data() + sizeof(float), &_size, sizeof(glm::vec3));
	//rawData.insert(rawData.end(), boxData.begin(), boxData.end());
	std::vector<uint8_t> boxData(2 * sizeof(float) + sizeof(glm::vec3)); // some strange sizeof() values for memory alignment on GPU side
	memcpy(boxData.data() + sizeof(float), &_size, sizeof(glm::vec3));
	rawData.insert(rawData.end(), boxData.begin(), boxData.end());

	return rawData;
}

bool Box::modifySelectedPrimitiveUI(std::string primitiveName)
{
	bool modified = false;

	// basic transform and color modification via UI
	modified = Primitive::modifySelectedPrimitiveUI(primitiveName);

	// special box modification via UI

	// draggable widget for size
	if (ImGui::DragFloat3((primitiveName + " size").c_str(), &_size.x, 0.01f, 0.1f))
	{
		if (_size.x < 0.001f)
			_size.x = 0.001f;
		if (_size.y < 0.001f)
			_size.y = 0.001f;
		if (_size.z < 0.001f)
			_size.z = 0.001f;

		modified = true;
	}

	return modified;
}