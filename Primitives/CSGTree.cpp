#include "renderer/opengl/Primitives/CSGTree.hpp"

#include <stack>

CSGTree::CSGTree() : _root{ nullptr } {};

CSGTree::CSGTree(std::shared_ptr<CSGNode> rootNode) : _root{ rootNode }
{}

CSGTree::CSGTree(std::shared_ptr<Primitive> rootPrimitive) : _root{CSGNode::makePrimitive(rootPrimitive)}
{}

int CSGTree::height() const
{
	if (isEmpty())
		return 0;
	else
		return _root->height();
}

int CSGTree::nbNode() const
{
	if (isEmpty())
		return 0;
	else
		return 1 + _root->nbOfChild();
}

bool CSGTree::isValid() const
{
	if (_root == nullptr)
		return true;
	else
		return _root->isValid();
}

std::vector<uint8_t> CSGTree::treeRawData() const
{
	if (_root == nullptr)
		return std::vector<uint8_t>(0);
	return _root->rawData();
}

int CSGTree::nbOfPrimitive() const
{
	if (_root == nullptr)
		return 0;
	int nbPrimitive = 0;
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(_root);

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();
		if (currentNode->isLeaf())
		{
			nbPrimitive++;
		}
		else
		{
			if (currentNode->getSecondChild() != nullptr)
				stackNode.push(currentNode->getSecondChild());
			if (currentNode->getFirstChild() != nullptr)
				stackNode.push(currentNode->getFirstChild());
		}
	}
	return nbPrimitive;
}

int CSGTree::nbOfPrimitiveByType(Primitive::PrimitiveType type) const
{
	if (_root == nullptr)
		return 0;

	int nbPrimitive = 0;
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(_root);

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();
		if (currentNode->isLeaf() && currentNode->getPrimitive()->getType() == type)
		{
			nbPrimitive++;
		}
		else
		{
			if (currentNode->getSecondChild() != nullptr)
				stackNode.push(currentNode->getSecondChild());
			if (currentNode->getFirstChild() != nullptr)
				stackNode.push(currentNode->getFirstChild());
		}
	}
	return nbPrimitive;
}

std::shared_ptr<CSGNode> CSGTree:: atPreorder(int preorderIdx) const
{
	if (_root == nullptr)
		return nullptr;
	else if (preorderIdx == 0)
		return _root;
	else
		return _root->atPreorderChild(preorderIdx);
}

std::shared_ptr<CSGNode> CSGTree::atPostorder(int postorderIdx) const
{
	if (_root == nullptr)
		return nullptr;
	else if (postorderIdx == nbNode() - 1)
		return _root;
	else
		return _root->atPostorderChild(postorderIdx);
}

std::shared_ptr<CSGNode> CSGTree::getLeafAtIndex(const int idx) const
{
	if (_root == nullptr)
		return nullptr;
	return _root->getLeafAtIndex(idx);
}

bool CSGTree::removeAtPreorderOperation(const int preorderIdx)
{
	if (preorderIdx == 0 && _root != nullptr && !_root->isLeaf())
	{
		_root = _root->getFirstChild();
		return true;
	}
	else
	{
		return _root->removeAtPreorderOperation(preorderIdx);
	}
}

bool CSGTree::unsafeRemoveAtPreorder(const int preorderIdx)
{
	if (_root == nullptr)
		return false;
	else if (preorderIdx == 0) {
                _root = nullptr;
                return true;
        }
	else
		return _root->unsafeRemoveAtPreorder(preorderIdx);
}



const std::vector<std::shared_ptr<const CSGNode>> CSGTree::getAllPrimitiveNodesByType(Primitive::PrimitiveType type) const
{
	std::vector<std::shared_ptr<const CSGNode>> primitives;
	if (_root == nullptr)
		return primitives;
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(_root);

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();
		if (currentNode->isLeaf() && currentNode->getPrimitive()->getType() == type)
		{
			primitives.push_back(currentNode);
		}
		else
		{
			if (currentNode->getSecondChild() != nullptr)
				stackNode.push(currentNode->getSecondChild());
			if (currentNode->getFirstChild() != nullptr)
				stackNode.push(currentNode->getFirstChild());
		}
	}
	return primitives;
}

const std::vector<std::shared_ptr<const CSGNode>> CSGTree::getAllPrimitiveNodes() const
{
	std::vector<std::shared_ptr<const CSGNode>> primitives;
	if (_root == nullptr)
		return primitives;
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(_root);

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();
		if (currentNode->isLeaf())
		{
			primitives.push_back(currentNode);
		}
		else
		{
			if (currentNode->getSecondChild() != nullptr)
				stackNode.push(currentNode->getSecondChild());
			if (currentNode->getFirstChild() != nullptr)
				stackNode.push(currentNode->getFirstChild());
		}
	}
	return primitives;
}

std::vector<uint8_t> CSGTree::rawDataByPrimitiveType(Primitive::PrimitiveType type) const // return the raw data of a given type of primitive, arranged for being sent to a shader as a SSBO
{
	std::vector<uint8_t> rawData;
	if (_root == nullptr)
		return rawData;
	std::stack<std::shared_ptr<const CSGNode>> stackNode;
	stackNode.push(_root);

	while (!stackNode.empty())
	{
		auto currentNode = stackNode.top();
		stackNode.pop();
		if (currentNode->isLeaf() && currentNode->getPrimitive()->getType() == type)
		{
			std::vector<uint8_t> currentNodeData = currentNode->getPrimitive()->rawData();
			rawData.insert(rawData.end(), currentNodeData.begin(), currentNodeData.end());
		}
		else
		{
			if (currentNode->getSecondChild() != nullptr)
				stackNode.push(currentNode->getSecondChild());
			if (currentNode->getFirstChild() != nullptr)
				stackNode.push(currentNode->getFirstChild());
		}
	}
	return rawData;
}

std::ostream& operator<<(std::ostream& os, const CSGTree& tree)
{
	os << "Tree of height " << tree.height() << ":\n\n";
	os << tree.getRoot();
	return os;
}