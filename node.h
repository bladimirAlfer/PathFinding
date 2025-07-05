//
// Created by juan-diego on 3/11/24.
//

#ifndef HOMEWORK_GRAPH_NODE_H
#define HOMEWORK_GRAPH_NODE_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>

// Color por defecto de un vertice (usado por SFML)
sf::Color default_node_color = sf::Color(150, 40, 50);
// Radio por defecto de un vertice (usado por SFML)
float default_radius = 0.2f;


struct Edge;


// *
// ---- Node ----
// Esta estructura contiene la informacion de un Nodo
//
// Variables miembro
//     - id            : Identificador de un vertice, debe ser irrepetible entre vertices
//     - coord         : La coordenada donde se encuentra el vertice (usado por SFML)
//     - edges         : El conjunto de aristas asociadas al vertice
//     - color         : Color del vertice (usado por SFML)
//     - radius        : Radio del vertice (usado por SFML)
//
// Funciones miembro
//     - parse_csv     : Lee los vertices desde el csv
//     - draw          : Dibuja el vertice instanciado
//     - reset         : Setea 'color' y 'radius' a sus valores por defecto
// *
struct Node {
    std::size_t id;
    sf::Vector2f coord;
    std::vector<Edge *> edges {};

    sf::Color color = default_node_color;
    float radius = default_radius;

    explicit Node(std::size_t id, float x, float y) : id(id), coord(x, y) {}

    static void parse_csv(const std::string &nodes_path, std::map<std::size_t, Node *> &nodes) {
        std::ifstream file(nodes_path);
        if (!file.is_open()) return;

        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty())
                continue;

            std::stringstream ss(line);
            std::string id_str, y_str, x_str;
            if (!std::getline(ss, id_str, ',')) continue;
            if (!std::getline(ss, y_str, ',')) continue;
            if (!std::getline(ss, x_str, ',')) continue;

            try {
                std::size_t identifier = std::stoull(id_str);
                float y = std::stof(y_str);
                float x = std::stof(x_str);
                nodes[identifier] = new Node(identifier, y, x);
            }
            catch (const std::exception&) {
                continue;
            }
        }
    }

    void draw(sf::RenderWindow &window) const {
        sf::CircleShape point(radius);
        point.setPosition(coord);
        point.setFillColor(color);

        window.draw(point);
    }

    void reset() {
        color = default_node_color;
        radius = default_radius;
    }
};


#endif //HOMEWORK_GRAPH_NODE_H
