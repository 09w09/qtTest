#include "mainwindow.h"

#include "AGraphicsItem.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  m_view = new QGraphicsView();
  m_scene = new QGraphicsScene();
  m_scene->setSceneRect(-1000, -1000, 2000, 2000);
  m_view->setScene(m_scene);

  AGraphicsItem *item = new AGraphicsItem();
  m_scene->addItem(item);

  setCentralWidget(m_view);
}

MainWindow::~MainWindow() { delete ui; }
