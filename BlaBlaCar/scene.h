#ifndef SCENE_H
#define SCENE_H
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QDebug>
#include <QString>
#include "statemachine.h"

namespace StateMachineBlaBlaCar
{
class Scene
{
public:
    Scene(StateMachine<std::string, Allocator<std::string>>* stateMachine);
    QGraphicsScene* drawScene();
private:
    StateMachine<std::string, Allocator<std::string>>* stateMachine;
    void drawText(QGraphicsScene* scene, QPoint point, QString text);
    void drawEllipse(QGraphicsScene* scene, QPoint center, int radius);
    void drawState(QGraphicsScene* scene, QPoint center, int radius, QString text);
    void drawSuperState(QGraphicsScene* scene, int x0, int y0, int dx, int dy, QString text);
    QPoint* getQuarterCenters(int x0, int y0, int dx, int dy);
};
}
#endif // SCENE_H
