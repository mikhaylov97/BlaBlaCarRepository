#ifndef SCENE_H
#define SCENE_H
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QDebug>
#include <QString>
#include "statemachine.h"
#include "ui_mainwindow.h"

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
    void drawText(QGraphicsScene* scene, QPoint point, QString text);
    void drawEllipse(QGraphicsScene* scene, QPoint center, int radius, bool isActive);
    void drawState(QGraphicsScene* scene, QPoint center, int radius, QString text, bool isActive);
    void drawSuperState(QGraphicsScene* scene, int x0, int y0, int dx, int dy, QString text, bool isActive);
    QPoint* getQuarterCenters(int x0, int y0, int dx, int dy);
};
}
#endif // SCENE_H
