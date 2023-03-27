#include <QApplication>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QGraphicsPolygonItem>
#include <QDebug>

class RegionItem : public QGraphicsPolygonItem {
public:
    RegionItem(QPolygonF polygon) : QGraphicsPolygonItem(polygon) {}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        qDebug() << "Region clicked!";
        QGraphicsPolygonItem::mousePressEvent(event);
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Leer el archivo de texto que define el grafo
    QFile file("grafo.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo de texto.";
        return 1;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList list = line.split(" ");
    int num_vertices = list[0].toInt();
    int num_aristas = list[1].toInt();

    QList<QPair<int, int>> aristas;
    for (int i = 0; i < num_aristas; i++) {
        line = in.readLine();
        list = line.split(" ");
        int u = list[0].toInt();
        int v = list[1].toInt();
        aristas.append(qMakePair(u, v));
    }

    file.close();

    // Coloreo de vértices
    QList<int> colores(num_vertices);
    QList<bool> usados(num_vertices);
    for (int i = 0; i < num_vertices; i++) {
        usados[i] = false;
    }
    int num_colores = 0;
    for (int i = 0; i < num_vertices; i++) {
        if (usados[i]) {
            continue;
        }
        colores[i] = num_colores++;
        usados[i] = true;
        for (int j = i + 1; j < num_vertices; j++) {
            if (usados[j]) {
                continue;
            }
            bool adyacentes = false;
            for (auto arista : aristas) {
                if ((arista.first == i && arista.second == j) ||
                    (arista.first == j && arista.second == i)) {
                    adyacentes = true;
                    break;
                }
            }
            if (!adyacentes) {
                colores[j] = colores[i];
                usados[j] = true;
            }
        }
    }

    // Crear una escena y agregar los nodos y aristas del grafo
    QGraphicsScene scene;

    // Dibujar los vértices como círculos en la escena
    QList<QGraphicsEllipseItem*> nodos;
    for (int i = 0; i < num_vertices; i++) {
        QGraphicsEllipseItem *nodo = new QGraphicsEllipseItem(-5, -5, 10, 10);
        nodo->setBrush(QBrush(Qt::white));
        nodo->setPos(qrand() % 400, qrand() % 400);
        scene.addItem(nodo);
        nodos.append(nodo);
    }

    // Dibujar las aristas como líneas en la escena