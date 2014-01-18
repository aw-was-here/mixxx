#include <QtDebug>
#include <QIcon>

#include "widget/wcombobox.h"

WComboBox::WComboBox(QWidget* pParent)
        : QComboBox(pParent),
          WBaseWidget(this) {
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotCurrentIndexChanged(int)));
}

WComboBox::~WComboBox() {
}

void WComboBox::setup(QDomNode node, const SkinContext& context) {
    // Load pixmaps for associated states
    QDomNode state = context.selectNode(node, "State");
    while (!state.isNull()) {
        if (state.isElement() && state.nodeName() == "State") {
            if (!context.hasNode(state, "Number")) {
                qDebug() << "WComboBox ignoring <State> without <Number> node.";
                continue;
            }

            int iState = context.selectInt(state, "Number");
            QString text = context.selectString(state, "Text");
            QString icon = context.selectString(state, "Icon");
            addItem(QIcon(icon), text, QVariant(iState));
        }
        state = state.nextSibling();
    }
}

bool WComboBox::event(QEvent* pEvent) {
    if (pEvent->type() == QEvent::ToolTip) {
        updateTooltip();
    }
    return QWidget::event(pEvent);
}

void WComboBox::onConnectedControlValueChanged(double v) {
    int index = findData(static_cast<int>(v));
    if (index != -1) {
        setCurrentIndex(index);
    }
}

void WComboBox::slotCurrentIndexChanged(int index) {
    setControlParameterLeftDown(index);
    setControlParameterLeftUp(index);
}
