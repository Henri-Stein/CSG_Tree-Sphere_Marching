#include "renderer/opengl/Primitives/CSGNode.hpp"
#include <stack>
#include <limits>

unsigned int CSGNode::_nbNodeCreated = 0;

std::string CSGNode::nodeTypeToString(const NodeType& nodeType)
{
	switch (nodeType)
	{
	case CSGNode::NodeType::Primitive:
		return "Primitive";
		break;
	case CSGNode::NodeType::Union:
		return "Union";
		break;
	case CSGNode::NodeType::Intersection:
		return "Intersection";
		break;
	case CSGNode::NodeType::Complement:
		return "Complement";
		break;
	case CSGNode::NodeType::Difference:
		return "Difference";
		break;
	default:
		return "DEFAULT";
		break;
	}
}

int CSGNode::height() const // return the height of the tree from this node
{
	if (isLeaf())
		return 1;
	else
	{
		int firstChildHeight = _children.first == nullptr ? 0 : _children.first->height();
		int secondChildHeight = _children.second == nullptr ? 0 : _children.second->height();
		return firstChildHeight >= secondChildHeight ? 1 + firstChildHeight : 1 + secondChildHeight;
	}
}

int CSGNode::nbOfChild() const
{
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(std::make_shared<CSGNode>(*this));
	
	int nbChild = 0;
	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();

		if (currentNode->getSecondChild() != nullptr)
		{
			nbChild++;
			stackNode.push(currentNode->getSecondChild());
		}
		if (currentNode->getFirstChild() != nullptr)
		{
			nbChild++;
			stackNode.push(currentNode->getFirstChild());
		}
	}
	return nbChild;
}


int CSGNode::nbOfLeaves() const
{
	if (isLeaf())
		return 1;
	else
	{
		return _children.first->nbOfLeaves() + (_type != NodeType::Complement ? _children.second->nbOfLeaves() : 0);
	}
}

bool CSGNode::isValid() const
{
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(std::make_shared<CSGNode>(*this));

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();

		if (!currentNode->isLeaf() && currentNode->getFirstChild() == nullptr)
			return false;
		else if (!currentNode->isLeaf() && currentNode->getType() != NodeType::Complement && currentNode->getSecondChild() == nullptr)
			return false;

		if (currentNode->getSecondChild() != nullptr)
		{
			stackNode.push(currentNode->getSecondChild());
		}
		if (currentNode->getFirstChild() != nullptr)
		{
			stackNode.push(currentNode->getFirstChild());
		}
	}
	return true;
}


CSGNode::NodePtr CSGNode::atPreorderChild(const int preorderIdx)
{
	CSGNode::NodePtr nodePtr = nullptr;
	auto getNode = [&nodePtr](CSGNode::NodePtr node) -> bool
	{
		nodePtr = node;
		return node != nullptr;
	};

	executeAtPreorder(preorderIdx, getNode);
	return nodePtr;
}

bool CSGNode::removeAtPreorderOperationStep(int& idx, const int preorderIdx)
{
	idx++;
	if (_children.first)
	{
		if (idx == preorderIdx)
		{
			if (_children.first->isLeaf()) // Can't remove leaf, as it would make an operation node miss an operator
				return false;
			else // If the node is an operation
			{
				_children.first = _children.first->_children.first;
				return true;
			}
		}
		else
		{
			auto resLeft = _children.first->removeAtPreorderOperationStep(idx, preorderIdx);
			if (resLeft)
				return resLeft;
		}
	}

	if (_children.second)
	{
		if (idx == preorderIdx)
		{
			if (_children.second->isLeaf()) // Can't remove leaf, as it would make an operation node miss an operator
				return false;
			else // If the node is an operation
			{
				_children.second = _children.second->_children.first; // The second child of the current node become the first child of its second child
				return true;
			}
		}
		else
		{
			auto resRight = _children.second->removeAtPreorderOperationStep(idx, preorderIdx);
			if (resRight)
				return resRight;
		}
	}

	return false;
}

bool CSGNode::removeAtPreorderOperation(const int preorderIdx)
{
	if (preorderIdx <= 0) // Can't remove the root node
		return false;
	int idx = 0;
	return removeAtPreorderOperationStep(idx, preorderIdx);
}


bool CSGNode::unsafeRemoveAtPreorderStep(int& idx, const int preorderIdx)
{
	idx++;
	if (_children.first)
	{
		if (idx == preorderIdx)
		{
			_children.first = _children.first->_children.first;
			return true;
		}
		else
		{
			if (const bool resLeft = _children.first->unsafeRemoveAtPreorderStep(idx, preorderIdx))
				return resLeft;
		}
	}

	if (_children.second)
	{
		if (idx == preorderIdx)
		{
			_children.second = _children.second->_children.first;
			return true;
		}
		else
		{
			if (const bool resRight = _children.second->unsafeRemoveAtPreorderStep(idx, preorderIdx))
				return resRight;
		}
	}

	return false;
}

bool CSGNode::unsafeRemoveAtPreorder(const int preorderIdx)
{
	if (preorderIdx <= 0) // Can't remove the root node
		return false;
	int idx = 0;
	return unsafeRemoveAtPreorderStep(idx, preorderIdx);
}

//bool CSGNode::removeAtPreorderOperationStep(int& idx, const int preorderIdx)
//{
//	idx++;
//	if (_children.first)
//	{
//		if (idx == preorderIdx)
//		{
//			if (_children.first->isLeaf()) // Can't remove leaf, as it would make an operation node miss an operator
//				return false;
//			else // If the node is an operation
//			{
//				_children.first = _children.first->_children.first;
//				return true;
//			}
//		}
//		else
//		{
//			auto resLeft = _children.first->removeAtPreorderOperationStep(idx, preorderIdx);
//			if (resLeft)
//				return resLeft;
//		}
//	}
//
//	if (_children.second)
//	{
//		if (idx == preorderIdx)
//			return _children.second;
//		else
//		{
//			auto resRight = _children.second->atPreorderChildStep(idx, preorderIdx);
//			if (resRight)
//				return resRight;
//		}
//	}
//
//	return nullptr;
//}
//
//bool CSGNode::removeAtPreorderOperation(const int preorderIdx)
//{
//	if (preorderIdx < 0)
//		return false;
//	int idx = 0;
//	return removeAtPreorderOperationStep(idx, preorderIdx);
//}

/*
* The idea behind this recursive algorithm is that, instead of checking if the current node is the one we are looking for, we delegate this responsibility to the parent, which checks if any of its children match the target node.
* WHY CHECK IN THE PARENT NODE AND NOT IN THE CURRENT NODE NAIVELY ? -> Because in C++, it is not safe to return a smart pointer pointing to 'this' directly.
*/
CSGNode::NodePtr CSGNode::atPostorderChildStep(int& idx, const int& postorderIndex) const
{
	if (_children.first) // If has a first child
	{
		if (!_children.first->isLeaf()) // If first child is not a leaf
		{
			auto resLeft = _children.first->atPostorderChildStep(idx, postorderIndex); // Visit the first child
			if (resLeft) // If we found the target node in the first child
				return resLeft;
		}
		if (idx == postorderIndex) // If the first child is the target node
			return _children.first;
		idx++; // Increment idx after finishing visiting the first child
	}

	if (_children.second) // If had a second child
	{
		if (!_children.second->isLeaf()) // If second child is not a leaf
		{
			auto resRight = _children.second->atPostorderChildStep(idx, postorderIndex); // Visit the second child
			if (resRight) // If we found the target node in the second child
				return resRight;
		}
		if (idx == postorderIndex) // If the second child is the target node
			return _children.second;
		idx++; // Increment idx after finishing visiting the second child
	}

	return nullptr; // If we are here, it means none of the children either is or contains the target node.
}


CSGNode::NodePtr CSGNode::atPostorderChild(const int postorderIdx) const
{
	if (postorderIdx < 0)
		return nullptr;
	int idx = 0;
	return atPostorderChildStep(idx, postorderIdx);
}

std::shared_ptr<CSGNode> CSGNode::getLeafAtIndex(const int idx)
{
	std::stack<std::shared_ptr<CSGNode>> stackNode;
	stackNode.push(std::make_shared<CSGNode>(*this));

	int leavesCounter = 0;
	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();

		if (currentNode->isLeaf())
		{
			if (leavesCounter == idx)
				return currentNode;
			leavesCounter++;
		}

		else
		{
			if (currentNode->getSecondChild() != nullptr)
			{
				stackNode.push(currentNode->getSecondChild());
			}
			if (currentNode->getFirstChild() != nullptr)
			{
				stackNode.push(currentNode->getFirstChild());
			}
		}
	}
	return nullptr;
}

std::vector<uint8_t> CSGNode::rawDataStep(int& ite, std::vector<unsigned int>& primitiveCounter) const // Recursive postorder traversal with variable incrementation
{

	ShaderNodeData nodeData{};

	std::vector<uint8_t> resultRawData(0);

	if (isLeaf()) // Primitive node
	{
		/*
		* Build the node structure
		*/
		switch (_primitive->getType())
		{
			case Primitive::PrimitiveType::Sphere:
				nodeData.type = SHADER_TYPE_SPHERE;
				break;
			case Primitive::PrimitiveType::Torus:
				nodeData.type = SHADER_TYPE_TORUS;
				break;
			case Primitive::PrimitiveType::Cylinder:
				nodeData.type = SHADER_TYPE_CYLINDER;
				break;
			case Primitive::PrimitiveType::Box:
				nodeData.type = SHADER_TYPE_BOX;
				break;
                        default:
                          break;
		}
		nodeData.leftChildIndex = -1;
		nodeData.rightChildIndex = -1;
		nodeData.primitiveIndex = primitiveCounter[static_cast<int>(_primitive->getType())];
		primitiveCounter[static_cast<int>(_primitive->getType())]++;
	}
	else // Operation node
	{
		/*
		* Retrieve child data recursively, in the postorder traversal.
		* Careful, this instruction must be called at the beginning of the block because the variable 'ite' will be changed by side effect in 'rawDataStep'.
		*/
		auto firstRawData = _children.first->rawDataStep(ite, primitiveCounter);
		auto secondRawData = std::vector<uint8_t>(0);
		if (getType() != CSGNode::NodeType::Complement)
		{
			secondRawData = _children.second->rawDataStep(ite, primitiveCounter);
		}

		/*
		* Build the node structure
		*/
		switch (getType())
		{
		case CSGNode::NodeType::Union:
			nodeData.type = SHADER_TYPE_UNION;
			break;
		case CSGNode::NodeType::Intersection:
			nodeData.type = SHADER_TYPE_INTERSECTION;
			break;
		case CSGNode::NodeType::Complement:
			nodeData.type = SHADER_TYPE_COMPLEMENTARY;
			break;
		case CSGNode::NodeType::Difference:
			nodeData.type = SHADER_TYPE_DIFFERENCE;
			break;
                default:
                        break;
		}

		nodeData.leftChildIndex = ite - (1 + nbOfRightChild()); // Computationally expensive, maybe think about storing the number of right child as a member variable
		if (getType() != CSGNode::NodeType::Complement)
		{
			nodeData.rightChildIndex = ite - 1; // Postorder traversal, so the right child is the last node that has been visited
		}
		else
		{
			nodeData.rightChildIndex = -1; // if type Complement, then no right child
		}
		nodeData.primitiveIndex = -1;


		/*
		* Append the raw data of children at the beginning of the resulting raw data.
		*/
		resultRawData.insert(resultRawData.begin(), secondRawData.begin(), secondRawData.end());
		resultRawData.insert(resultRawData.begin(), firstRawData.begin(), firstRawData.end()); // The raw data of the first child must be placed first in the final result, then followed by the raw data of the second child
	}

	/*
	* Format current node to raw uint8_t data
	*/
	std::vector<uint8_t> currentRawData(4 * sizeof(int));

	memcpy(currentRawData.data(), &nodeData.type, sizeof(int));
	memcpy(currentRawData.data() + sizeof(int) , &nodeData.leftChildIndex, sizeof(int));
	memcpy(currentRawData.data() + 2 * sizeof(int), &nodeData.rightChildIndex, sizeof(int));
	memcpy(currentRawData.data() + 3 * sizeof(int), &nodeData.primitiveIndex, sizeof(int));

	/*
	* Append the raw data of the current node, increment the 'ite' reference and finally return the raw data
	*/
	resultRawData.insert(resultRawData.end(), currentRawData.begin(), currentRawData.end());
	//std::cout << "ite: " << ite << "  ; nodeType = " << nodeTypeToString(getType()) << " | leftChildIndex = " << nodeData.leftChildIndex << " ; rightChildIndex = " << nodeData.rightChildIndex << std::endl;
	ite++; 
	return resultRawData;
}

std::vector<uint8_t> CSGNode::rawData() const
{
	std::vector<unsigned int> primitiveCounter(static_cast<int>(Primitive::PrimitiveType::MAX) + 1, 0); // Keep track of the number of primitive already evaluated (useful to get the index of the last primitive in the primitive SSBO)
	int ite = 0;
	return rawDataStep(ite, primitiveCounter);
}

std::ostream& operator<<(std::ostream& os, const CSGNode::NodePtr& node)
{

	std::stack<std::pair<std::shared_ptr<const CSGNode>, int>> stackNode;
	int currentDepth = 0;
	stackNode.push({ node, currentDepth}); // Second variable contains the depth of the node according to the root

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		if (currentNode.second > 0)
		{
			int columnLength = 2;
			for (int j = 0; j < columnLength; j++)
			{
				for (int i = 0; i < currentNode.second; i++)
				{
					os << std::string(int(i>0) * 3, ' ') << "|";
				}
				if (j < columnLength - 1)
				{
					os <<"\n";
				}
				else
				{
					os << "--->";
				}
			}
		}

		switch (currentNode.first->getType())
		{
		case CSGNode::NodeType::Primitive:
			std::cout << "\033[35m"; // Purple color
			break;
		case CSGNode::NodeType::Union:
			std::cout << "\033[32m"; // Green color
			break;
		case CSGNode::NodeType::Intersection:
			std::cout << "\033[33m"; // Yellow color
			break;
		case CSGNode::NodeType::Complement:
			std::cout << "\033[31m"; // red color
			break;
		case CSGNode::NodeType::Difference:
			std::cout << "\033[36m";
			break;
		default:
			break;
		}

		os << currentNode.first->_name /*CSGNode::nodeTypeToString(currentNode.first->getType())*/ << "\033[0m\n";

		stackNode.pop();

		if (currentNode.first->getSecondChild() != nullptr)
			stackNode.push({currentNode.first->getSecondChild(), currentNode.second+1});
		if (currentNode.first->getFirstChild() != nullptr)
			stackNode.push({ currentNode.first->getFirstChild(), currentNode.second+1});
	}
    return os;
}