#include "renderer/opengl/Primitives/CSGTreeTest.hpp"

#include "renderer/opengl/Primitives/CSGTree.hpp"
#include "renderer/opengl/Primitives/CSGNode.hpp"
#include "renderer/opengl/Primitives/Sphere.hpp"
#include "renderer/opengl/Primitives/Torus.hpp"
#include "renderer/opengl/Primitives/Cylinder.hpp"
#include "renderer/opengl/Primitives/Box.hpp"



/*
* Build this tree:
*
*           U
*          / \
*	      I   \
*        / \   \
*       U   C   \
*      / \  |    \
*     S   B C     T
*
*/
CSGTree CSGTreeTest::buildSimpleTree() const
{
	Sphere s{};
	Box b{};
	Cylinder c{};
	Torus t{};

	/*
	* The number at the end of each node name indicate its index in the Postorder traversal (parcours suffixe)
	*/
	auto prim0 = CSGNode::makePrimitive(std::make_shared<Sphere>(s));
	auto prim1 = CSGNode::makePrimitive(std::make_shared<Box>(b));
	auto union2 = CSGNode::makeUnion(prim0, prim1);

	auto prim3 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c));
	auto complem4 = CSGNode::makeComplement(prim3);

	auto inter5 = CSGNode::makeIntersection(union2, complem4);

	auto prim6 = CSGNode::makePrimitive(std::make_shared<Torus>(t));
	auto union7 = CSGNode::makeUnion(inter5, prim6);

	return CSGTree{ union7 };
}

/*
* Build this tree:
* 
*			   C
*              |
*              U
*			  / \
*            I   \
*           / \   \
*		   /   \   \
*		  /		I   \
*		 /	   / \   \
*       U     U   \   \
*      / \   / \   \   \
*     C   T C   B   S   C
*
*/
CSGTree CSGTreeTest::buildMediumTree() const
{
	Sphere s0{};
	Box b0{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Torus t0{};

	/*
	* The number at the end of each node name indicate its index in the Postorder traversal (parcours suffixe)
	*/
	auto prim0 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim1 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union2 = CSGNode::makeUnion(prim0, prim1);

	auto prim3 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim4 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto union5 = CSGNode::makeUnion(prim3, prim4);

	auto prim6 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(union5, prim6);
	auto intersection8 = CSGNode::makeIntersection(union2, intersection7);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union10 = CSGNode::makeUnion(intersection8, prim9);

	auto complement11 = CSGNode::makeComplement(union10);

	return CSGTree{ complement11 };
}



/*
* Build this tree:
* 
*               C
*               |
*               U
*              / \
*             /   \
*            U     \
*           / \     \
*          I   \	 \
*		  / \   \     \
*        /   \   \     \
*       /     I   \     \
*      /     / \   \     \
*     U     D   \   \     I
*    / \   / \   \   \   / \
*   C   T C   B   S   C B   T
*/
CSGTree CSGTreeTest::buildComplexTree() const
{
	Sphere s0{};
	Torus t0{};
	Torus t1{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Box b0{};
	Box b1{};

	/*
	* The number at the end of each node name indicate its index in the Postorder traversal (parcours suffixe)
	*/
	auto prim0 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim1 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union2 = CSGNode::makeUnion(prim0, prim1);

	auto prim3 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim4 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto difference5 = CSGNode::makeDifference(prim3, prim4);

	auto prim6 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(difference5, prim6);
	auto intersection8 = CSGNode::makeIntersection(union2, intersection7);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union10 = CSGNode::makeUnion(intersection8, prim9);

	auto prim11 = CSGNode::makePrimitive(std::make_shared<Box>(b1));
	auto prim12 = CSGNode::makePrimitive(std::make_shared<Torus>(t1));
	auto intersection13 = CSGNode::makeIntersection(prim11, prim12);

	auto union14 = CSGNode::makeUnion(union10, intersection13);

	auto complement15 = CSGNode::makeComplement(union14);

	return CSGTree{ complement15 };


	/*
	* Another way of building the Tree, using the interface from CSGTree instead of CSGNode
	*/

	//CSGTree tree1{ std::make_shared<Cylinder>(c0) };
	//tree1.addUnion(std::make_shared<Torus>(t0));

	//CSGTree tree2{ std::make_shared<Cylinder>(c1) };
	//tree2.addDifference(std::make_shared<Box>(b0));
	//tree2.addIntersection(std::make_shared<Sphere>(s0));

	//tree1.combineTreeByIntersection(tree2);
	//tree1.addUnion(std::make_shared<Cylinder>(c2));

	//CSGTree tree3{ std::make_shared<Box>(b1) };
	//tree3.addIntersection(std::make_shared<Torus>(t1));
	//
	//tree1.combineTreeByUnion(tree3);
	//tree1.addComplement();
	//return tree1;
}

void CSGTreeTest::performAllTests() const
{
	std::cout << "\nStarted executing CSGTree tests\n___________________________________________________________________________\n" << std::endl;
	std::cout << "Test nbOfPrimitives: " << (testNbOfPrimitive() ? "success" : "failure") << std::endl;
	std::cout << "Test height: " << (testHeight() ? "success" : "failure") << std::endl;
	std::cout << "Test getAllPrimitives: " << (testGetAllPrimitiveNodes() ? "success" : "failure") << std::endl;
	std::cout << "Test getAllPrimitivesByType: " << (testGetAllPrimitiveNodesByType() ? "success" : "failure") << std::endl;
	std::cout << "Test atPostorder: " << (testAtPostorder() ? "success" : "failure") << std::endl;
	std::cout << "Test atPreorder: " << (testAtPreorder() ? "success" : "failure") << std::endl;
	std::cout << "Test treeRawData: " << (testTreeRawData() ? "success" : "failure") << std::endl;
	std::cout << "Test removeAtPreorderOperation: " << (testRemoveAtPreorderOperation() ? "success" : "failure") << std::endl;
	std::cout << "Test emptyTree: " << (testEmptyTree() ? "success" : "failure") << std::endl;
	std::cout << "Test getLeafAtIndex: " << (testGetLeafAtIndex() ? "success" : "failure") << std::endl;

	printSampleTree();
	std::cout << "\nFinished testing CSGTree\n___________________________________________________________________________\n" << std::endl;

	//CSGTree simpleTree = buildSimpleTree();
	//auto root = simpleTree.getRoot();

	//auto getHeight = [](CSGNode::NodePtr node) -> bool
	//{
	//	return node->height();
	//};

	//auto getHeightPlusA = [](CSGNode::NodePtr node, int a) -> bool
	//{
	//	return node->height() + a;
	//};

	//CSGNode::NodePtr nodePtr = nullptr;
	//auto getNode = [&nodePtr](CSGNode::NodePtr node) -> bool
	//{
	//	nodePtr = node;
	//	return node != nullptr;
	//};

	//std::cout << "Height = " << root->executeAtPreorder(1, getHeight) << std::endl;
	//std::cout << "Height + a = " << root->executeAtPreorder<int>(1, getHeightPlusA, 5) << std::endl;
	//std::cout << root->executeAtPreorder(0, getNode) << std::endl;
	//std::cout << (nodePtr != nullptr);
}

bool CSGTreeTest::testNbOfPrimitive() const
{
	CSGTree complexTree = buildComplexTree();
	int nbSpheres = complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Sphere);
	int nbToruses = complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Torus);
	int nbCylinders = complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Cylinder);
	int nbBoxes = complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Box);
	int totalNbPrimitives = complexTree.nbOfPrimitive();

	return nbSpheres == 1 && nbToruses == 2 && nbCylinders == 3 && nbBoxes == 2 && totalNbPrimitives == 8;
}

bool CSGTreeTest::testHeight() const
{
	CSGTree simpleTree = buildSimpleTree();
	CSGTree mediumTree = buildMediumTree();
	CSGTree complexTree = buildComplexTree();


	return simpleTree.height() == 4 && mediumTree.height() == 6 && complexTree.height() == 7;
}

bool CSGTreeTest::testGetLeafAtIndex() const
{
	Sphere s0{};
	Torus t0{};
	Torus t1{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Box b0{};
	Box b1{};

	auto prim0 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim1 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union2 = CSGNode::makeUnion(prim0, prim1);

	auto prim3 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim4 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto difference5 = CSGNode::makeDifference(prim3, prim4);

	auto prim6 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(difference5, prim6);
	auto intersection8 = CSGNode::makeIntersection(union2, intersection7);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union10 = CSGNode::makeUnion(intersection8, prim9);

	auto prim11 = CSGNode::makePrimitive(std::make_shared<Box>(b1));
	auto prim12 = CSGNode::makePrimitive(std::make_shared<Torus>(t1));
	auto intersection13 = CSGNode::makeIntersection(prim11, prim12);

	auto union14 = CSGNode::makeUnion(union10, intersection13);

	auto complement15 = CSGNode::makeComplement(union14);

	CSGTree tree{ complement15 };

	return tree.getLeafAtIndex(0) == prim0 && tree.getLeafAtIndex(2) == prim3 && tree.getLeafAtIndex(5) == prim9 && tree.getLeafAtIndex(1000) == nullptr;
}


bool CSGTreeTest::testGetAllPrimitiveNodes() const
{
	CSGTree simpleTree = buildSimpleTree();
	CSGTree mediumTree = buildMediumTree();
	CSGTree complexTree = buildComplexTree();

	return static_cast<int>(simpleTree.getAllPrimitiveNodes().size()) ==
                   simpleTree.nbOfPrimitive() && static_cast<int>(mediumTree
                                    .getAllPrimitiveNodes().size()) ==
                   mediumTree.nbOfPrimitive() && static_cast<int>(complexTree.getAllPrimitiveNodes().size()) == complexTree.nbOfPrimitive();
}

bool CSGTreeTest::testGetAllPrimitiveNodesByType() const
{
	CSGTree simpleTree = buildSimpleTree();
	CSGTree mediumTree = buildMediumTree();
	CSGTree complexTree = buildComplexTree();

	std::vector<Primitive::PrimitiveType> primType{};
	primType.push_back(Primitive::PrimitiveType::Sphere);
	primType.push_back(Primitive::PrimitiveType::Torus);
	primType.push_back(Primitive::PrimitiveType::Cylinder);
	primType.push_back(Primitive::PrimitiveType::Box);

	for (auto pt : primType)
	{
		if (static_cast<int>(simpleTree.getAllPrimitiveNodesByType(pt)
                                .size()) != simpleTree.nbOfPrimitiveByType(pt)
				|| static_cast<int>(mediumTree
                                      .getAllPrimitiveNodesByType(pt).size())
                     != mediumTree.nbOfPrimitiveByType(pt)
				|| static_cast<int>(complexTree
                                      .getAllPrimitiveNodesByType(pt).size()) != complexTree.nbOfPrimitiveByType(pt))
			return false;
	}

	return true;
}

bool CSGTreeTest::testAtPreorder() const
{
	Sphere s0{};
	Torus t0{};
	Torus t1{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Box b0{};
	Box b1{};

	auto prim5 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim6 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union4 = CSGNode::makeUnion(prim5, prim6);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim10 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto difference8 = CSGNode::makeDifference(prim9, prim10);

	auto prim11 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(difference8, prim11);
	auto intersection3 = CSGNode::makeIntersection(union4, intersection7);

	auto prim12 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union2 = CSGNode::makeUnion(intersection3, prim12);

	auto prim14 = CSGNode::makePrimitive(std::make_shared<Box>(b1));
	auto prim15 = CSGNode::makePrimitive(std::make_shared<Torus>(t1));
	auto intersection13 = CSGNode::makeIntersection(prim14, prim15);

	auto union1 = CSGNode::makeUnion(union2, intersection13);

	auto complement0 = CSGNode::makeComplement(union1);

	CSGTree tree{ complement0 };

	bool noNullPtrCheck = true;
	for (int i = 0; i < tree.nbNode(); ++i)
	{
		if (tree.atPreorder(i) == nullptr)
			std::cout << i << std::endl;
		noNullPtrCheck = noNullPtrCheck && tree.atPreorder(i) != nullptr;
	}

	bool validityCheck = tree.getRoot() == tree.atPreorder(0)
		&& tree.atPreorder(15) == prim15
		&& tree.atPreorder(14) == prim14
		&& tree.atPreorder(13) == intersection13
		&& tree.atPreorder(12) == prim12
		&& tree.atPreorder(11) == prim11
		&& tree.atPreorder(10) == prim10
		&& tree.atPreorder(9) == prim9
		&& tree.atPreorder(8) == difference8
		&& tree.atPreorder(7) == intersection7
		&& tree.atPreorder(6) == prim6
		&& tree.atPreorder(5) == prim5
		&& tree.atPreorder(4) == union4
		&& tree.atPreorder(3) == intersection3
		&& tree.atPreorder(2) == union2
		&& tree.atPreorder(1) == union1
		&& tree.atPreorder(0) == complement0;

	return noNullPtrCheck && validityCheck;
}

bool CSGTreeTest::testAtPostorder() const
{
	Sphere s0{};
	Torus t0{};
	Torus t1{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Box b0{};
	Box b1{};

	auto prim0 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim1 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union2 = CSGNode::makeUnion(prim0, prim1);

	auto prim3 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim4 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto difference5 = CSGNode::makeDifference(prim3, prim4);

	auto prim6 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(difference5, prim6);
	auto intersection8 = CSGNode::makeIntersection(union2, intersection7);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union10 = CSGNode::makeUnion(intersection8, prim9);

	auto prim11 = CSGNode::makePrimitive(std::make_shared<Box>(b1));
	auto prim12 = CSGNode::makePrimitive(std::make_shared<Torus>(t1));
	auto intersection13 = CSGNode::makeIntersection(prim11, prim12);

	auto union14 = CSGNode::makeUnion(union10, intersection13);

	auto complement15 = CSGNode::makeComplement(union14);

	CSGTree tree{ complement15 };

	bool noNullPtrCheck = true;
	for (int i = 0; i < tree.nbNode(); ++i)
	{
		if (tree.atPostorder(i) == nullptr)
			std::cout << i << std::endl;
		noNullPtrCheck = noNullPtrCheck && tree.atPostorder(i) != nullptr;
	}

	bool validityCheck = tree.getRoot() == tree.atPostorder(tree.nbNode() - 1)
		&& tree.atPostorder(15) == complement15
		&& tree.atPostorder(14) == union14
		&& tree.atPostorder(13) == intersection13
		&& tree.atPostorder(12) == prim12
		&& tree.atPostorder(11) == prim11
		&& tree.atPostorder(10) == union10
		&& tree.atPostorder(9) == prim9
		&& tree.atPostorder(8) == intersection8
		&& tree.atPostorder(7) == intersection7
		&& tree.atPostorder(6) == prim6
		&& tree.atPostorder(5) == difference5
		&& tree.atPostorder(4) == prim4
		&& tree.atPostorder(3) == prim3
		&& tree.atPostorder(2) == union2
		&& tree.atPostorder(1) == prim1
		&& tree.atPostorder(0) == prim0;

	return noNullPtrCheck && validityCheck;
}

bool CSGTreeTest::testRemoveAtPreorderOperation() const
{
	Sphere s0{};
	Torus t0{};
	Torus t1{};
	Cylinder c0{};
	Cylinder c1{};
	Cylinder c2{};
	Box b0{};
	Box b1{};

	auto prim5 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c0));
	auto prim6 = CSGNode::makePrimitive(std::make_shared<Torus>(t0));
	auto union4 = CSGNode::makeUnion(prim5, prim6);

	auto prim9 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c1));
	auto prim10 = CSGNode::makePrimitive(std::make_shared<Box>(b0));

	auto difference8 = CSGNode::makeDifference(prim9, prim10);

	auto prim11 = CSGNode::makePrimitive(std::make_shared<Sphere>(s0));
	auto intersection7 = CSGNode::makeIntersection(difference8, prim11);
	auto intersection3 = CSGNode::makeIntersection(union4, intersection7);

	auto prim12 = CSGNode::makePrimitive(std::make_shared<Cylinder>(c2));

	auto union2 = CSGNode::makeUnion(intersection3, prim12);

	auto prim14 = CSGNode::makePrimitive(std::make_shared<Box>(b1));
	auto prim15 = CSGNode::makePrimitive(std::make_shared<Torus>(t1));
	auto intersection13 = CSGNode::makeIntersection(prim14, prim15);

	auto union1 = CSGNode::makeUnion(union2, intersection13);

	auto complement0 = CSGNode::makeComplement(union1);

	CSGTree tree{ complement0 };


	tree.removeAtPreorderOperation(0);
	bool rootCheck = tree.getRoot() == union1;

	tree.removeAtPreorderOperation(6);
	bool removeNodeCheck = intersection3->getSecondChild() == difference8;

	int nbNode = tree.nbNode();
	tree.removeAtPreorderOperation(-1);
	tree.removeAtPreorderOperation(nbNode);
	bool outOfRangeCheck = nbNode == tree.nbNode();

	return rootCheck && removeNodeCheck && outOfRangeCheck;
}

bool CSGTreeTest::testTreeRawData() const
{
	CSGTree complexTree = buildComplexTree();
	auto rawData = complexTree.treeRawData();
	//std::cout << (sizeof(int) + 3 * sizeof(float) + sizeof(glm::vec3) + 3 * sizeof(int) + 2 * sizeof(float) == 3 * 4 * 4) << std::endl;
	//std::cout << rawData.size() << std::endl;
	//std::cout << rawData.size() / (3 * 4 * 4) << std::endl;

	bool dataSizeCheck = (static_cast<int>(rawData.size()) == complexTree
                                                                          .nbNode() * 1 * 4 * 4)  && (rawData.size() == complexTree.nbNode() * (4 * sizeof(int))); // rawData.size should be equal to 'complexTree.nbNode() * 1 * 4 * 4' because it should contains 'complexTree.nbNode()' elements of 1 blocks of 4 Bytes (1 * 4 * 4 bits)

	bool dataValidityCheck = true;

	auto buildNodeStruct = [rawData](int i) // Return a CSGNode::ShaderNodeData struct builded from the bits data in rawData and at index i
	{
		int index = i * 1 * 4 * 4;
		CSGNode::ShaderNodeData nodeData;
		memcpy(&nodeData.type, &(rawData[index]), sizeof(int));
		memcpy(&nodeData.leftChildIndex, &(rawData[index]) + sizeof(int), sizeof(int));
		memcpy(&nodeData.rightChildIndex, &(rawData[index]) + 2 * sizeof(int), sizeof(int));
		memcpy(&nodeData.primitiveIndex, &(rawData[index]) + 3 * sizeof(int), sizeof(int));
		return nodeData;
	};

	auto spheres = complexTree.getAllPrimitiveNodesByType(Primitive::PrimitiveType::Sphere);
	auto toruses = complexTree.getAllPrimitiveNodesByType(Primitive::PrimitiveType::Torus);
	auto cylinders = complexTree.getAllPrimitiveNodesByType(Primitive::PrimitiveType::Cylinder);
	auto boxes = complexTree.getAllPrimitiveNodesByType(Primitive::PrimitiveType::Box);
	for (int i = 0; i < complexTree.nbNode(); i++)
	{
		CSGNode::ShaderNodeData nodeData = buildNodeStruct(i);
		std::shared_ptr<CSGNode> csgNode = complexTree.atPostorder(i);
		switch (csgNode->_type)
		{
		case CSGNode::NodeType::Primitive:
			switch (csgNode->_primitive->getType())
			{
			case Primitive::PrimitiveType::Sphere:
				dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_SPHERE;
				dataValidityCheck = dataValidityCheck && spheres[nodeData.primitiveIndex]->_primitive == csgNode->_primitive;
				break;
			case Primitive::PrimitiveType::Torus:
				dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_TORUS;
				dataValidityCheck = dataValidityCheck && toruses[nodeData.primitiveIndex]->_primitive == csgNode->_primitive;
				break;
			case Primitive::PrimitiveType::Cylinder:
				dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_CYLINDER;
				dataValidityCheck = dataValidityCheck && cylinders[nodeData.primitiveIndex]->_primitive == csgNode->_primitive;
				break;
			case Primitive::PrimitiveType::Box:
				dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_BOX;
				dataValidityCheck = dataValidityCheck && boxes[nodeData.primitiveIndex]->_primitive == csgNode->_primitive;
				break;
			default:
				break;
			}
			break;
		case CSGNode::NodeType::Union:
			dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_UNION;
			break;
		case CSGNode::NodeType::Intersection:
			dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_INTERSECTION;
			break;
		case CSGNode::NodeType::Complement:
			dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_COMPLEMENTARY;
			break;
		case CSGNode::NodeType::Difference:
			dataValidityCheck = dataValidityCheck && nodeData.type == SHADER_TYPE_DIFFERENCE;
			break;
		default:
			break;
		}

		if (csgNode->getType() == CSGNode::NodeType::Primitive)
		{
			dataValidityCheck = dataValidityCheck && nodeData.leftChildIndex == -1 && nodeData.rightChildIndex == -1;
		}
		else
		{
			dataValidityCheck = dataValidityCheck && nodeData.primitiveIndex == -1;
			dataValidityCheck = dataValidityCheck && *(complexTree.atPostorder(nodeData.leftChildIndex)) == *(csgNode->getFirstChild()); // check that first child is at the correct index
			if (csgNode->getType() != CSGNode::NodeType::Complement)
			{
				dataValidityCheck = dataValidityCheck && *(complexTree.atPostorder(nodeData.rightChildIndex)) == *(csgNode->getSecondChild()); // CHeck that second child is at the correct index
			}
		}
	}

	return dataSizeCheck && dataValidityCheck;
}

bool CSGTreeTest::testEmptyTree() const
{
	CSGTree emptyTree{};
	std::vector<Primitive::PrimitiveType> primitiveTypes{};
	primitiveTypes.push_back(Primitive::PrimitiveType::Sphere);
	primitiveTypes.push_back(Primitive::PrimitiveType::Torus);
	primitiveTypes.push_back(Primitive::PrimitiveType::Cylinder);
	primitiveTypes.push_back(Primitive::PrimitiveType::Box);

	bool isEmptyCheck = emptyTree.isEmpty();
	bool nbNodeCheck = emptyTree.nbNode() == 0;
	bool heightCheck = emptyTree.height() == 0;
	bool nbOfPrimitiveCheck = emptyTree.nbOfPrimitive() == 0;

	bool nbOfPrimitiveByTypeCheck = true;
	bool getAllPrimitiveNodesByTypeCheck = true;
	bool rawDataByPrimitiveTypeCheck = true;
	for (const auto& pt : primitiveTypes)
	{
		nbOfPrimitiveByTypeCheck = nbOfPrimitiveByTypeCheck && emptyTree.nbOfPrimitiveByType(pt) == 0;
		getAllPrimitiveNodesByTypeCheck = getAllPrimitiveNodesByTypeCheck && emptyTree.getAllPrimitiveNodesByType(pt).size() == 0;
		rawDataByPrimitiveTypeCheck = rawDataByPrimitiveTypeCheck && emptyTree.rawDataByPrimitiveType(pt).size() == 0;
	}

	bool getRootCheck = emptyTree.getRoot() == nullptr;
	bool getAllPrimitiveNodesCheck = emptyTree.getAllPrimitiveNodes().size() == 0;
	bool getNodeAtPostorderIdxCheck = emptyTree.atPostorder(0) == nullptr && emptyTree.atPostorder(1) == nullptr && emptyTree.atPostorder(-1) == nullptr;
	bool treeRawDataCheck = emptyTree.treeRawData().size() == 0;
	bool atPostorderCheck = emptyTree.atPostorder(0) == nullptr && emptyTree.atPostorder(1568) == nullptr && emptyTree.atPostorder(-1) == nullptr;
	bool atPreorderCheck = emptyTree.atPreorder(0) == nullptr && emptyTree.atPreorder(1568) == nullptr && emptyTree.atPreorder(-1) == nullptr;


	return isEmptyCheck && nbNodeCheck && heightCheck && nbOfPrimitiveCheck && nbOfPrimitiveByTypeCheck && getAllPrimitiveNodesByTypeCheck && rawDataByPrimitiveTypeCheck && getRootCheck
		&& getAllPrimitiveNodesCheck && getNodeAtPostorderIdxCheck && treeRawDataCheck && atPostorderCheck && atPreorderCheck;
}

void CSGTreeTest::printSampleTree() const
{
	std::cout << "***********************************************************************************************************************" << std::endl;
	std::cout << "************************************************** Print sample tree **************************************************" << std::endl;
	std::cout << "***********************************************************************************************************************" << std::endl;

	CSGTree complexTree = buildComplexTree();

	std::cout << "Nb of primitives: " << complexTree.nbOfPrimitive() << std::endl;
	std::cout << "Nb of sphere: " << complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Sphere) << std::endl;
	std::cout << "Nb of torus: " << complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Torus) << std::endl;;
	std::cout << "Nb of cylinder: " << complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Cylinder) << std::endl;;
	std::cout << "Nb of box: " << complexTree.nbOfPrimitiveByType(Primitive::PrimitiveType::Box) << std::endl;;

	std::cout << complexTree << std::endl;
}