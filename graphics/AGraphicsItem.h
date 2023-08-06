#ifndef AGRAPHICSITEM_H
#define AGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QMap>

class AGraphicsItem : public QGraphicsItem {
private:
  // 当前调整大小的模式 0000->上下左右
  enum TransformState {
    TF_Cen = 0x00,
    TF_Top = 0x08,    // 1000
    TF_Bottom = 0x04, // 0100
    TF_Left = 0x02,   // 0010
    TF_Right = 0x01,  // 0001

    TF_TopR = TF_Top | TF_Right,       // 1001
    TF_BottomR = TF_Bottom | TF_Right, // 0101
    TF_BottomL = TF_Bottom | TF_Left,  // 0110
    TF_TopL = TF_Top | TF_Left,        // 1010
  };

public:
  AGraphicsItem();
  QRectF boundingRect() const override;

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QMap<TransformState, QRectF> rectWhere();

private:
  qreal m_border;
  QSizeF m_grapSize;
  QSizeF m_minSize;
  TransformState m_tfState;
};

#endif // AGRAPHICSITEM_H
