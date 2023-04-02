#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui/QPainter>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Grafo {
public:
    Grafo() {}

    void leer_archivo(std::string filename) {
        std::ifstream archivo(filename);
        std::string linea;

        while (std::getline(archivo, linea)) {
            int v, w;
            std::istringstream ss(linea);
            ss >> v;
            while (ss >> w) {
                vertices[v].push_back(w);
                vertices[w].push_back(v);
            }
        }
    }

    std::unordered_map<int, std::vector<int>> vertices;
};

class GrafoWidget : public QWidget {
public:
    GrafoWidget(QWidget* parent = nullptr)
            : QWidget(parent)
            , grafo()
            , regions()
            , colors()
    {
        grafo.leer_archivo("grafo.txt");

        // Determinar las regiones del grafo
        int region = 0;
        std::unordered_set<int> visitados;
        for (const auto& [v, vecinos] : grafo.vertices) {
            if (visitados.find(v) == visitados.end()) {
                DFS(v, visitados, region);
                ++region;
            }
        }

        // Colorear el grafo
        std::unordered_set<int> disponibles;
        for (const auto& [v, vecinos] : grafo.vertices) {
            for (int i = 0; i < region; ++i) {
                disponibles.insert(i);
            }
            for (const auto& w : vecinos) {
                disponibles.erase(regions[v]);
            }
            int color = *disponibles.begin();
            colors[regions[v]] = QColor::fromHsv(color * 360 / region, 255, 255);
            disponibles.clear();
        }
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // Dibujar los vértices y aristas del grafo
        for (const auto& [v, vecinos] : grafo.vertices) {
            for (const auto& w : vecinos) {
                painter.drawLine(v * 10, w * 10, w * 10, v * 10);
            }
            painter.drawEllipse(v * 10 - 5, v * 10 - 5, 10, 10);
        }

        // Dibujar las regiones del grafo
        painter.setBrush(QBrush(Qt::gray));
        for (const auto& [v, region] : regions) {
            painter.setBrush(QBrush(colors[region]));
            painter.drawEllipse(v * 10 - 5, v * 10 - 5, 10, 10);
        }
    }

private:
    void DFS(int v, std::unordered_set<int>& visitados, int region) {
        visitados.insert(v);
        regions[v] = region;
        for (const auto& w : grafo.vertices[v]) {
            if (visitados.find(w) == visitados.end()) {
                DFS(w, visitados, region);
            }
        }
    }

    Grafo grafo;
    std::unordered_map<int, int> regions;
    std::unordered_map<int, QColor> colors;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Crear la ventana principal
    MainWindow w;

    // Leer el archivo de grafo planar
    QString fileName = QFileDialog::getOpenFileName(&w, "Abrir archivo", "", "Archivos de texto (*.txt)");
    if (fileName.isEmpty()) {
        QMessageBox::critical(&w, "Error", "No se ha seleccionado ningún archivo.");
        return -1;
    }

    // Crear el grafo a partir del archivo
    Grafo grafo = leerGrafoDesdeArchivo(fileName);

    // Asignar el grafo a la ventana principal
    w.setGrafo(grafo);

    // Ejecutar el algoritmo de coloreado
    int numColores = colorearGrafo(grafo);
    w.setNumColores(numColores);

    // Mostrar la ventana principal
    w.show();

    return a.exec();
}
