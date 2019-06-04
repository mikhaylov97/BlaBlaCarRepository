#include "scene.h"

using namespace StateMachineBlaBlaCar;

Scene::Scene(StateMachine<std::string, Allocator<std::string>>* stateMachine)
{
    this->stateMachine = stateMachine;
}

QGraphicsScene* Scene::drawScene() {
    QGraphicsScene* scene = new QGraphicsScene();
    QPen solidPen = QPen(Qt::black, 1, Qt::SolidLine);
    QPen dashDotPen = QPen(Qt::black, 1, Qt::DashDotLine);
    int superStateCount = stateMachine->get_states_number();

    int subStateRadius = 10;
    QPoint sceneCenter = QPoint(300, 300);
    int sceneRadius = 150;

    int dx = 250;
    int dy = 250;

    int currentI = 0;
    for(auto stateIterator = stateMachine->get_states_vector()->begin(); stateIterator != stateMachine->get_states_vector()->end(); ++stateIterator) {

        QPoint currentStateCenter = QPoint(sceneCenter.x() + sin(2 * M_PI / superStateCount * currentI) * sceneRadius,
                                           sceneCenter.y() + cos(2 * M_PI / superStateCount * currentI) * sceneRadius);
        if (stateIterator->get_states()->size() != 0) {
            int x0 = currentStateCenter.x() - dx / 2;
            int y0 = currentStateCenter.y() - dy / 2;
            drawSuperState(scene, x0, y0, dx, dy, QString::fromStdString(stateIterator->get_value()));
            scene->addLine(x0, y0 + dy / 2, x0 + dx, y0 + dy / 2, dashDotPen);
            scene->addLine(x0 + dx / 2, y0, x0 + dx / 2, y0 + dy, dashDotPen);
            QPoint* points = getQuarterCenters(x0, y0, dx, dy);
            int passengersCount = 4;
            for (int i = 0; i < passengersCount; i++){
                int currentJ = 0;
                int subStateStatesCount = stateIterator->get_states()->size();
                for(auto substatesIterator = stateIterator->get_states()->begin(); substatesIterator != stateIterator->get_states()->end(); ++substatesIterator) {
                    QPoint currentSubStateCenter = QPoint(points[i].x() + sin(2 * M_PI / subStateStatesCount * currentJ) * dx / 6,
                                                  points[i].y() + cos(2 * M_PI / subStateStatesCount * currentJ) * dy / 6);
                    drawState(scene, currentSubStateCenter, subStateRadius, QString::fromStdString(substatesIterator->get_value()));
                    currentJ++;
                }
            }
        } else {
            drawState(scene, currentStateCenter, subStateRadius, QString::fromStdString(stateIterator->get_value()));
        }
        currentI++;
    }
    return scene;
}

void Scene::drawState(QGraphicsScene *scene, QPoint center, int radius, QString text) {
    drawEllipse(scene, center, radius);
    drawText(scene, center + QPoint(-radius, radius / 2), text);
}

void Scene::drawSuperState(QGraphicsScene *scene, int x0, int y0, int dx, int dy, QString text) {
    scene->addRect(x0,y0,dx,dy);
    drawText(scene, QPoint(x0 + dx / 2, y0 + dy), text);
}

void Scene::drawText(QGraphicsScene* scene, QPoint point, QString text) {
    QGraphicsTextItem * io = new QGraphicsTextItem;
    io->setPos(point);
    io->setPlainText(text);
    scene->addItem(io);
}

void Scene::drawEllipse(QGraphicsScene *scene, QPoint center, int radius) {
    scene->addEllipse(QRectF(center - QPoint(radius, radius), center + QPoint(radius, radius)));
}

QPoint* Scene::getQuarterCenters(int x0, int y0, int dx, int dy) {
    QPoint quarterCenter1 = QPoint(x0 + dx / 4, y0 + dy / 4);
    QPoint quarterCenter2 = QPoint(x0 + 3 * dx / 4, y0 + dy / 4);
    QPoint quarterCenter3 = QPoint(x0 + dx / 4, y0 + 3 * dy / 4);
    QPoint quarterCenter4 = QPoint(x0 + 3 * dx / 4, y0 + 3 * dy / 4);
    return new QPoint[4]{quarterCenter1, quarterCenter2, quarterCenter3, quarterCenter4};
}
