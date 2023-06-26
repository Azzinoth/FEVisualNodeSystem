#include "VisualNode.h"

std::unordered_map<std::string, VisualNodeChildFunc>& VisualNode::GetChildClasses()
{
	static std::unordered_map<std::string, VisualNodeChildFunc> ChildClasses;
	return ChildClasses;
}

VisualNode::VisualNode(const std::string ID)
{
	this->ID = ID;
	if (ID.empty())
		this->ID = APPLICATION.GetUniqueHexID();

	SetSize(ImVec2(200, 80));
	SetName("VisualNode");
	Type = "VisualNode";
}

VisualNode::VisualNode(const VisualNode& Src)
{
	ParentArea = Src.ParentArea;
	ID = APPLICATION.GetUniqueHexID();
	Position = Src.Position;
	Size = Src.Size;

	ClientRegionMin = Src.ClientRegionMin;
	ClientRegionMax = Src.ClientRegionMax;

	Name = Src.Name;
	Type = Src.Type;
	bShouldBeDestroyed = false;

	LeftTop = Src.LeftTop;
	RightBottom = Src.RightBottom;

	TitleBackgroundColor = Src.TitleBackgroundColor;
	TitleBackgroundColorHovered = Src.TitleBackgroundColorHovered;

	for (size_t i = 0; i < Src.Input.size(); i++)
	{
		Input.push_back(new NodeSocket(this, Src.Input[i]->GetType(), Src.Input[i]->GetName(), false));
	}

	for (size_t i = 0; i < Src.Output.size(); i++)
	{
		Output.push_back(new NodeSocket(this, Src.Output[i]->GetType(), Src.Output[i]->GetName(), true));
	}
}

VisualNode::~VisualNode()
{
	for (int i = 0; i < static_cast<int>(Input.size()); i++)
	{
		delete Input[i];
		Input.erase(Input.begin() + i, Input.begin() + i + 1);
		i--;
	}

	for (int i = 0; i < static_cast<int>(Output.size()); i++)
	{
		delete Output[i];
		Output.erase(Output.begin() + i, Output.begin() + i + 1);
		i--;
	}
}

std::string VisualNode::GetID()
{
	return ID;
}

ImVec2 VisualNode::GetPosition() const
{
	return Position;
}

void VisualNode::SetPosition(const ImVec2 NewValue)
{
	Position = NewValue;
}

ImVec2 VisualNode::GetSize() const
{
	if (GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
		return ImVec2(NODE_DIAMETER, NODE_DIAMETER);
	
	return Size;
}

void VisualNode::SetSize(const ImVec2 NewValue)
{
	Size = NewValue;
}

std::string VisualNode::GetName()
{
	return Name;
}

void VisualNode::SetName(const std::string NewValue)
{
	if (NewValue.size() > VISUAL_NODE_NAME_MAX_LENGHT)
		return;

	Name = NewValue;
}

void VisualNode::AddSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return;

	if (Socket->bOutput)
		Output.push_back(Socket);
	else
		Input.push_back(Socket);
}

void VisualNode::Draw()
{
}

void VisualNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{

}

bool VisualNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	// Socket can't connect to itself.
	if (OwnSocket == CandidateSocket)
		return false;

	// Nodes can't connect to themselves.
	if (CandidateSocket->GetParent() == this)
		return false;

	// Output can't connect to output and input can't connect to input.
	if (OwnSocket->bOutput == CandidateSocket->bOutput)
		return false;

	return true;
}

std::string VisualNode::GetType() const
{
	return Type;
}

Json::Value VisualNode::ToJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["nodeType"] = Type;
	result["position"]["x"] = Position.x;
	result["position"]["y"] = Position.y;
	result["size"]["x"] = Size.x;
	result["size"]["y"] = Size.y;
	result["name"] = Name;

	for (size_t i = 0; i < Input.size(); i++)
	{
		result["input"][std::to_string(i)]["ID"] = Input[i]->GetID();
		result["input"][std::to_string(i)]["name"] = Input[i]->GetName();
		result["input"][std::to_string(i)]["type"] = Input[i]->GetType();
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		result["output"][std::to_string(i)]["ID"] = Output[i]->GetID();
		result["output"][std::to_string(i)]["name"] = Output[i]->GetName();
		result["output"][std::to_string(i)]["type"] = Output[i]->GetType();
	}

	return result;
}

void VisualNode::FromJson(Json::Value Json)
{
	ID = Json["ID"].asCString();
	Type = Json["nodeType"].asCString();
	Position.x = Json["position"]["x"].asFloat();
	Position.y = Json["position"]["y"].asFloat();
	Size.x = Json["size"]["x"].asFloat();
	Size.y = Json["size"]["y"].asFloat();
	Name = Json["name"].asCString();

	const std::vector<Json::String> InputsList = Json["input"].getMemberNames();
	for (size_t i = 0; i < Input.size(); i++)
	{
		delete Input[i];
		Input[i] = nullptr;
	}
	Input.resize(InputsList.size());
	for (size_t i = 0; i < InputsList.size(); i++)
	{
		const std::string ID = Json["input"][std::to_string(i)]["ID"].asCString();
		const std::string name = Json["input"][std::to_string(i)]["name"].asCString();
		const std::string type = Json["input"][std::to_string(i)]["type"].asCString();

		Input[i] = new NodeSocket(this, type, name, false);
		Input[i]->ID = ID;
	}

	const std::vector<Json::String> OutputsList = Json["output"].getMemberNames();
	for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
		Output[i] = nullptr;
	}
	Output.resize(OutputsList.size());
	for (size_t i = 0; i < OutputsList.size(); i++)
	{
		const std::string ID = Json["output"][std::to_string(i)]["ID"].asCString();
		const std::string name = Json["output"][std::to_string(i)]["name"].asCString();
		const std::string type = Json["output"][std::to_string(i)]["type"].asCString();

		Output[i] = new NodeSocket(this, type, name, true);
		Output[i]->ID = ID;
	}
}

void VisualNode::UpdateClientRegion()
{
	float LongestInputSocketTextW = 0.0f;
	for (size_t i = 0; i < Input.size(); i++)
	{
		const ImVec2 TextSize = ImGui::CalcTextSize(Input[i]->GetName().c_str());
		if (TextSize.x > LongestInputSocketTextW)
			LongestInputSocketTextW = TextSize.x;
	}

	float LongestOutputSocketTextW = 0.0f;
	for (size_t i = 0; i < Output.size(); i++)
	{
		const ImVec2 TextSize = ImGui::CalcTextSize(Output[i]->GetName().c_str());
		if (TextSize.x > LongestOutputSocketTextW)
			LongestOutputSocketTextW = TextSize.x;
	}

	ClientRegionMin.x = LeftTop.x + NODE_SOCKET_SIZE * 5.0f + LongestInputSocketTextW + 2.0f;
	ClientRegionMax.x = RightBottom.x - NODE_SOCKET_SIZE * 5.0f - LongestOutputSocketTextW - 2.0f;

	ClientRegionMin.y = LeftTop.y + NODE_TITLE_HEIGHT + 2.0f;
	ClientRegionMax.y = RightBottom.y - 2.0f;
}

ImVec2 VisualNode::GetClientRegionSize()
{
	UpdateClientRegion();
	return ClientRegionMax - ClientRegionMin;
}

ImVec2 VisualNode::GetClientRegionPosition()
{
	UpdateClientRegion();
	return ClientRegionMin;
}

size_t VisualNode::InputSocketCount() const
{
	return Input.size();
}

size_t VisualNode::OutSocketCount() const
{
	return Output.size();
}

bool VisualNode::GetForcedOutSocketColor(ImColor& Color, const size_t SocketIndex) const
{
	if (SocketIndex < 0 || SocketIndex >= Output.size())
		return false;

	return Output[SocketIndex]->GetForcedConnectionColor(Color);
}

void VisualNode::SetForcedOutSocketColor(ImColor* NewValue, const size_t SocketIndex) const
{
	if (SocketIndex < 0 || SocketIndex >= Output.size())
		return;

	Output[SocketIndex]->SetForcedConnectionColor(NewValue);
}

std::vector<VisualNode*> VisualNode::GetConnectedNodes() const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Output.size(); i++)
	{
		for (size_t j = 0; j < Output[i]->Connections.size(); j++)
		{
			result.push_back(Output[i]->Connections[j]->GetParent());
		}
	}

	return result;
}

VisualNode* VisualNode::GetLogicallyNextNode()
{
	const auto Connected = GetConnectedNodes();
	if (!Connected.empty() && Connected[0] != nullptr)
		return GetConnectedNodes()[0];
	
	return nullptr;
}

void VisualNode::RegisterChildNodeClass(const VisualNodeChildFunc Functions, const std::string ClassName)
{
	if (Functions.JsonToObj != nullptr && Functions.CopyConstructor != nullptr && !ClassName.empty())
	{
		GetChildClasses()[ClassName] = Functions;
	}
}

VisualNode* VisualNode::ConstructChild(const std::string ChildClassName, const Json::Value Data)
{
	if (GetChildClasses().find(ChildClassName) == GetChildClasses().end())
		return nullptr;

	return GetChildClasses()[ChildClassName].JsonToObj(Data);
}

VisualNode* VisualNode::CopyChild(const std::string ChildClassName, VisualNode* Child)
{
	if (GetChildClasses().find(ChildClassName) == GetChildClasses().end())
		return nullptr;

	return GetChildClasses()[ChildClassName].CopyConstructor(*Child);
}

bool VisualNode::OpenContextMenu()
{
	return false;
}

VISUAL_NODE_STYLE VisualNode::GetStyle() const
{
	return Style;
}

void VisualNode::SetStyle(const VISUAL_NODE_STYLE NewValue)
{
	if (static_cast<int>(NewValue) < 0 || static_cast<int>(NewValue) >= 2)
		return;

	Style = NewValue;
}

bool VisualNode::IsHovered() const
{
	return bHovered;
}

void VisualNode::SetIsHovered(const bool NewValue)
{
	bHovered = NewValue;
}