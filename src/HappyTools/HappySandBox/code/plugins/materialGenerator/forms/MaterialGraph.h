#ifndef MaterialGraph_H
#define MaterialGraph_H

#include "forms/nodegraph/NodeGraph.h"

namespace Ui {
    class MaterialGraph;
}

namespace he {
namespace ge {
    class Property;
}
namespace ct {
    class ShaderGenerator;
} }

namespace hs {
class MaterialEditor;
class MaterialGeneratorNode;
class MaterialGraph : public NodeGraph
{
    Q_OBJECT
public:
    explicit MaterialGraph(QWidget* parent);
    ~MaterialGraph();

    void init(MaterialEditor* parent);

    he::ct::ShaderGenerator* getShaderGenerator() const { return m_Generator; }
    
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void compile();
    void onPropertyValueChanged(he::ge::Property* prop);

protected:
    virtual NodeGraphNode* createNode(const he::FixedString& type) override;
    virtual void destroyNode(NodeGraphNode* node) override;

    // Called when a connection is made between two connectors, returns if successful
    virtual bool connect(NodeGraphNodeConnector* const from, NodeGraphNodeConnector* const to) override;

    // If selection 
    virtual void onSelectionChanged();

private:
    he::ct::ShaderGenerator* m_Generator;
    MaterialEditor* m_Parent;
    MaterialGeneratorNode* m_ActiveRoot;
};

}
#endif // MAINWINDOW_H
