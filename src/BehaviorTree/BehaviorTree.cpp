#include "BehaviorTree.hpp"
#include "Blackboard.hpp"

// ----------------------------------------------------------------------------
BehaviorTree::BehaviorTree()
{
    m_blackboard = new Blackboard();
}

// ----------------------------------------------------------------------------
BehaviorTree::~BehaviorTree()
{
    // We may not create the root node, but we come to own it;
    delete m_rootNode;
    delete m_blackboard;
}

// ----------------------------------------------------------------------------
Node::Status BehaviorTree::Run()
{
    return m_rootNode->Update();
}

// ----------------------------------------------------------------------------
void BehaviorTree::SetRootNode(Node* rootNode_)
{
    m_rootNode = rootNode_;
}

// ----------------------------------------------------------------------------
Blackboard* BehaviorTree::GetBlackboard()
{
    return m_blackboard;
}
