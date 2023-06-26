#include "VisualNodeSocket.h"

NodeSocket::NodeSocket(VisualNode* Parent, const std::string Type, const std::string Name, bool bOutput)
{
	this->Parent = Parent;
	this->Type = Type;
	this->Name = Name;
	this->ID = APPLICATION.GetUniqueHexID();
	this->bOutput = bOutput;
}

std::string NodeSocket::GetID()
{
	return ID;
}

VisualNode* NodeSocket::GetParent() const
{
	return Parent;
}

std::vector<NodeSocket*> NodeSocket::GetConnections()
{
	return Connections;
}

std::string NodeSocket::GetName()
{
	return Name;
}

std::string NodeSocket::GetType() const
{
	return Type;
}

bool NodeSocket::GetForcedConnectionColor(ImColor& Color) const
{
	if (ForceColor == nullptr)
		return false;

	Color = *ForceColor;

	return true;
}

void NodeSocket::SetForcedConnectionColor(ImColor* NewValue)
{
	ForceColor = NewValue;
}

VisualNodeConnection::VisualNodeConnection(NodeSocket* Out, NodeSocket* In)
{
	this->Out = Out;
	this->In = In;
}