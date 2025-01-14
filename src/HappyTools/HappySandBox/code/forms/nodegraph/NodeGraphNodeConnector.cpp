//HappyEngine Copyright (C) 2011 - 2014  Evil Interactive
//
//This file is part of HappyEngine.
//
//    HappyEngine is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    HappyEngine is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with HappyEngine.  If not, see <http://www.gnu.org/licenses/>.
//
#include "HappySandBoxPCH.h"
#include "NodeGraphNodeConnector.h"

#include "NodeGraphEnums.h"
#include "NodeGraphNode.h"
#include "NodeGraphNodeAttachment.h"
#include "NodeGraph.h"

#include <Canvas2D.h>
#include <StructuredVisitor.h>

namespace hs {

NodeGraphNodeConnector::ConnectorStyle::ConnectorStyle()
    : m_ConnectorSize(16, 16)
{
    m_ConnectorBackgroundColor[eConnectorState_Normal] = he::Color(126, 109, 95, static_cast<he::uint8>(255));
    m_ConnectorBackgroundColor[eConnectorState_Hover] = m_ConnectorBackgroundColor[eConnectorState_Normal];
    m_ConnectorBackgroundColor[eConnectorState_Selected] = he::Color(171, 148, 129, static_cast<he::uint8>(255));

    m_ConnectorBorderColor[eConnectorState_Normal] = he::Color(149, 106, 79, static_cast<he::uint8>(255));
    m_ConnectorBorderColor[eConnectorState_Hover] = he::Color(197, 139, 104, static_cast<he::uint8>(255));
    m_ConnectorBorderColor[eConnectorState_Selected] = he::Color(255, 180, 135, static_cast<he::uint8>(255));
}

NodeGraphNodeConnector::NodeGraphNodeConnector(NodeGraphNodeAttachment* parent)
    : m_State(eConnectorState_Normal)
    , m_Parent(parent)
    , m_Type(eNodeGraphNodeConnectorType_Input)
{
    setLayoutMinSize(he::vec2(12, 12));
    setLayoutMaxSize(he::vec2(12, 12));
}

NodeGraphNodeConnector::~NodeGraphNodeConnector()
{

}

void NodeGraphNodeConnector::setConnectorStyle(const NodeGraphNodeConnector::ConnectorStyle& style)
{
    m_Style = style;
}

bool NodeGraphNodeConnector::pick( const he::vec2& worldPos ) const 
{
    return getLayoutBound().isInside(worldPos);
}

void NodeGraphNodeConnector::draw( const NodeGraphDrawContext& context )
{
    const he::RectI& bound(he::RectI(getLayoutBound().transform(context.transform)));
    context.canvas->setColor(m_Style.m_ConnectorBackgroundColor[m_State]);
    context.canvas->fillRect(bound);

    context.canvas->setColor(m_Style.m_ConnectorBorderColor[m_State]);
    context.canvas->strokeRect(bound);

    if (m_Type == eNodeGraphNodeConnectorType_Input && isConnected())
    {
        drawConnection(context, *m_Connections[0]);
    }
}

void NodeGraphNodeConnector::drawConnection( const NodeGraphDrawContext& context, const he::vec2& other )
{
    he::vec2 pos0(m_LayoutBound.x + m_LayoutBound.width / 2.0f, m_LayoutBound.y + m_LayoutBound.height / 2.0f);
    he::vec2 tan0, tan1;
    if (m_Type == eNodeGraphNodeConnectorType_Input)
        tan0 = he::vec2(-1, 0);
    else
        tan0 = he::vec2(1, 0);
    pos0 = context.transform * pos0;
    he::vec2 otherTransformd(context.transform * other);
    tan0 *= abs((pos0.x - otherTransformd.x) / 2.0f);
    tan1 = -tan0;

    context.canvas->setColor(he::Color(1.0f, 1.0f, 1.0f, 1.0f));
    context.canvas->fillCurve(pos0, tan0, tan1, otherTransformd, 2.0f);
}

void NodeGraphNodeConnector::drawConnection( const NodeGraphDrawContext& context, const NodeGraphNodeConnector& other )
{
    he::vec2 pos1(other.m_LayoutBound.x + m_LayoutBound.width / 2.0f, other.m_LayoutBound.y + m_LayoutBound.height / 2.0f);
    drawConnection(context, pos1);
}

bool NodeGraphNodeConnector::connect( NodeGraphNodeConnector* other )
{
    if (!m_Connections.contains(other))
    {
        m_Connections.add(other);
        other->connect(this);
        HE_ASSERT(m_Type == eNodeGraphNodeConnectorType_Output || m_Connections.size() < 2, "An input connector can only have one connection!");
        return true;
    }
    return false;
}

bool NodeGraphNodeConnector::disconnect(NodeGraphNodeConnector* other)
{
    size_t index(0);
    if (m_Connections.find(other, index))
    {
        m_Connections.removeAt(index);
        other->disconnect(this);
        return true;
    }
    return false;
}

void NodeGraphNodeConnector::disconnectAll()
{
    for (size_t i(0); i < m_Connections.size();)
    {
        NodeGraphNodeConnector* const connector(m_Connections[i]);
        disconnect(connector); // will do a find, but will always be index 0, this way we can always react to 'disconnect(NodeGraphNodeConnector* other)'
    }
}

void NodeGraphNodeConnector::visit( he::io::StructuredVisitor* const visitor )
{
    if (m_Type == eNodeGraphNodeConnectorType_Input)
    {
        if (visitor->enterNode(m_Id))
        {
            if (visitor->isReading())
                disconnectAll();
            visitor->visitCustomList<NodeGraphNodeConnector*>(he::HEFS::strConnections, m_Connections, 
            [this](he::io::StructuredVisitor* visitor, size_t /*index*/, NodeGraphNodeConnector*& val)
            {
                he::Guid nodeId(val->m_Parent->getParent()->getGuid());
                if (visitor->visit(HSFS::strNodeID, nodeId))
                {
                    he::FixedString connectionId(val->getId());
                    if (visitor->visit(HSFS::strConnectionID, connectionId))
                    {
                        if (visitor->isReading())
                        {
                            NodeGraphNode* node(m_Parent->getParent()->getParent()->getNode(nodeId));
                            if (node)
                            {
                                node->getAttachments().forEach([this, &connectionId](NodeGraphNodeAttachment* att)
                                {
                                    NodeGraphNodeConnector* connector(att->getNodeConnector());
                                    if (connector && connector->getId() == connectionId)
                                    {
                                        connector->connect(this);
                                    }
                                });
                            }
                        }
                    }
                }
            });
            visitor->exitNode(m_Id);
        }
    }
}

} //end namespace

