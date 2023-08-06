#include "AGraphicsItem.h"

#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

AGraphicsItem::AGraphicsItem()
    : m_border(5), m_grapSize(100, 100), m_minSize(40, 40) {
  setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable |
           QGraphicsItem::ItemSendsGeometryChanges |
           QGraphicsItem::ItemIsMovable);
  setAcceptHoverEvents(true);
}

QRectF AGraphicsItem::boundingRect() const {
  return QRectF(QPointF(0, 0), m_grapSize);
}

// 图元绘制事件：图元被选中时绘制边框、以及图元辅助线
void AGraphicsItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option, QWidget *) {

  painter->setRenderHint(QPainter::Antialiasing);

  qreal penW = 1.0 / painter->transform().m11();
  QRectF imgRect =
      QRectF(QPointF(m_border, m_border), m_grapSize - QSizeF(10, 10));

  // 真正的图形
  painter->setBrush(Qt::yellow);
  painter->drawRect(imgRect);

  // 如果图元被选中
  if (isSelected()) {
    QPen borderPen(QColor(0, 120, 215), penW * 2, Qt::DashLine);
    QPen PointPen(QColor(90, 157, 253), penW, Qt::SolidLine);

    // 绘出虚线边框
    painter->setPen(borderPen);
    painter->setBrush(Qt::transparent);
    painter->drawRect(imgRect);

    // 绘出8角
    painter->setPen(PointPen);
    painter->setBrush(QColor(242, 60, 0));
    const QMap<TransformState, QRectF> rectMap = rectWhere();
    for (const QRectF &rectf : rectMap.values()) {
      painter->drawRect(rectf);
    }
  }
}

// 鼠标悬停事件：选中时，悬停在图元不同地方鼠标显示效果不同
void AGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsItem::hoverMoveEvent(event);

  bool isHit = false;
  const QMap<TransformState, QRectF> rectMap = rectWhere();
  for (const TransformState tsf : rectMap.keys()) {
    if (rectMap[tsf].contains(event->pos())) {
      switch (tsf) {
      case TF_Top:
      case TF_Bottom: {
        setCursor(QCursor(Qt::SizeVerCursor));
      } break;
      case TF_Left:
      case TF_Right: {
        setCursor(QCursor(Qt::SizeHorCursor));
      } break;
      case TF_TopL:
      case TF_BottomR: {
        setCursor(QCursor(Qt::SizeFDiagCursor));
      } break;
      case TF_TopR:
      case TF_BottomL: {
        setCursor(QCursor(Qt::SizeBDiagCursor));
      } break;
      default: {
        setCursor(QCursor(Qt::SizeAllCursor));
      } break;
      }
      m_tfState = tsf;
      isHit = true;
    }
  }

  if (!isHit) {
    m_tfState = TF_Cen;
    setCursor(QCursor(Qt::ArrowCursor));
  }
}

void AGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (event->buttons() == Qt::LeftButton) {
    // 计算鼠标偏移量
    qreal dx = event->pos().x() - event->lastPos().x();
    qreal dy = event->pos().y() - event->lastPos().y();
    qreal x = pos().x();
    qreal y = pos().y();
    qreal w = m_grapSize.width();
    qreal h = m_grapSize.height();

    // 状态改变，只有允许调整时才会进去相应部分
    uint8_t flgs = uint8_t(m_tfState);
    if ((flgs & 0x01) == 0x01) { // 右
      w = w + dx;
      setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    if ((flgs >> 1 & 0x01) == 0x01) { // 左
      x = x + dx;
      w = w - dx;
      setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    if ((flgs >> 2 & 0x01) == 0x01) { // 下
      h = h + dy;
      setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    if ((flgs >> 3 & 0x01) == 0x01) { // 上
      y = y + dy;
      h = h - dy;
      setFlag(QGraphicsItem::ItemIsMovable, false);
    }

    if (flgs) {
      // 最小宽高像素
      if (w < 40)
        w = 40;
      if (h < 40)
        h = 40;

      // 界面及时做出反馈
      prepareGeometryChange();
      setPos(x, y);
      m_grapSize = QSize(w, h);
    } else {
      setFlag(QGraphicsItem::ItemIsMovable, true);
    }
  }
  QGraphicsItem::mouseMoveEvent(event);
}

QMap<AGraphicsItem::TransformState, QRectF> AGraphicsItem::rectWhere() {
  static QMap<TransformState, QRectF> rectMap;

  int x1 = 0;
  int x2 = m_grapSize.width() / 2 - m_border;
  int x3 = m_grapSize.width() - m_border * 2;
  int y1 = 0;
  int y2 = m_grapSize.height() / 2 - m_border;
  int y3 = m_grapSize.height() - m_border * 2;
  qreal borderWH = m_border * 2 / transform().m11();

  rectMap[TF_TopL] = QRectF(x1, y1, borderWH, borderWH);
  rectMap[TF_Top] = QRectF(x2, y1, borderWH, borderWH);
  rectMap[TF_TopR] = QRectF(x3, y1, borderWH, borderWH);
  rectMap[TF_Right] = QRectF(x3, y2, borderWH, borderWH);
  rectMap[TF_BottomR] = QRectF(x3, y3, borderWH, borderWH);
  rectMap[TF_Bottom] = QRectF(x2, y3, borderWH, borderWH);
  rectMap[TF_BottomL] = QRectF(x1, y3, borderWH, borderWH);
  rectMap[TF_Left] = QRectF(x1, y2, borderWH, borderWH);

  return rectMap;
}
