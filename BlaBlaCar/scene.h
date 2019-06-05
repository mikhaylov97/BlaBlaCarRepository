#ifndef SCENE_H
#define SCENE_H
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QDebug>
#include <QString>
#include "statemachine.h"
#include "ui_mainwindow.h"
#include <QMap>
#include <QPolygonF>

namespace StateMachineBlaBlaCar
{
class Scene
{
public:
    Scene(StateMachine<std::string, Allocator<std::string>>* stateMachine,
          Ui::MainWindow *ui);
    QGraphicsScene* drawScene();
private:
    StateMachine<std::string, Allocator<std::string>>* stateMachine;
    Ui::MainWindow *ui;
    void drawText(QGraphicsScene* scene, QPoint point, QString text,QFont font = QFont());
    void drawEllipse(QGraphicsScene* scene, QPoint center, int radius, bool isActive);
    void drawState(QGraphicsScene* scene, QPoint center, int radius, QString text, bool isActive);
    void drawSuperState(QGraphicsScene* scene, int x0, int y0, int dx, int dy, QString text, bool isActive);
    void drawTransitions(QGraphicsScene* scene, StateMachine<std::string, Allocator<std::string>>*, QMap<int, QPoint> map);
    QPoint* getQuarterCenters(int x0, int y0, int dx, int dy);
    QPolygonF getArrowForTransition(QPointF from, QPointF to);
};
}
#endif // SCENE_H
