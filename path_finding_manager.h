//
// Created by juan-diego on 3/29/24.
//

#ifndef HOMEWORK_GRAPH_PATH_FINDING_MANAGER_H
#define HOMEWORK_GRAPH_PATH_FINDING_MANAGER_H


#include "window_manager.h"
#include "graph.h"
#include <unordered_map>
#include <set>
#include <SFML/System.hpp>
#include <algorithm>
#include <unordered_set>
#include <chrono>

enum Algorithm {
    None,
    Dijkstra,
    AStar,
    BFS
};


//* --- PathFindingManager ---
//
// Esta clase sirve para realizar las simulaciones de nuestro grafo.
//
// Variables miembro
//     - path           : Contiene el camino resultante del algoritmo que se desea simular
//     - visited_edges  : Contiene todas las aristas que se visitaron en el algoritmo, notar que 'path'
//                        es un subconjunto de 'visited_edges'.
//     - window_manager : Instancia del manejador de ventana, es utilizado para dibujar cada paso del algoritmo
//     - src            : Nodo incial del que se parte en el algoritmo seleccionado
//     - dest           : Nodo al que se quiere llegar desde 'src'
//*
class PathFindingManager {
    WindowManager *window_manager;
    std::vector<sfLine> path;
    std::vector<sfLine> visited_edges;

    size_t visited_count = 0;
    std::unordered_map<Node*, Node*> last_parent;

    struct Entry {
        Node* node;
        double dist;

        bool operator < (const Entry& other) const {
            return dist < other.dist;
        }
    };

    static double heuristic(Node* a, Node* b) {
        double dx = a->coord.x - b->coord.x;
        double dy = a->coord.y - b->coord.y;
        return std::sqrt(dx*dx + dy*dy);
    }

    void dijkstra(Graph &graph) {
        // Mapa de distancias mínimas desde src
        std::unordered_map<Node*, double> dist;
        std::unordered_map<Node*, Node*> parent;

        // Inicializar distancias a +∞ y padres a nullptr
        for (auto & [id, node] : graph.nodes) {
            dist[node]   = std::numeric_limits<double>::infinity();
            parent[node] = nullptr;
        }
        // La distancia al origen es 0
        dist[src] = 0.0;

        // Min-heap de pares (distancia, nodo)
        auto cmp = [](const std::pair<double, Node*> &a,
                      const std::pair<double, Node*> &b) {
            return a.first > b.first;
        };
        std::priority_queue<
                std::pair<double, Node*>,
                std::vector<std::pair<double, Node*>>,
                decltype(cmp)
        > pq(cmp);

        pq.push({0.0, src});

        // Bucle principal de Dijkstra
        while (!pq.empty()) {
            auto [d_u, u] = pq.top(); pq.pop();
            ++visited_count;

            // Si ya tenemos mejor distancia, la ignoramos
            if (d_u > dist[u])
                continue;

            if (u == dest)
                break;

            for (Edge* e : u->edges) {
                Node* v = e->dest;
                double alt = dist[u] + e->length;
                if (alt < dist[v]) {
                    dist[v] = alt;
                    parent[v] = u;
                    pq.push({alt, v});
                    // Guardar arista explorada (para visualización)
                    visited_edges.emplace_back(
                            u->coord, v->coord,
                            sf::Color(200,200,200),
                            0.9f
                    );
                    //render();
                }
            }
        }

        last_parent = parent;

        set_final_path(parent);
    }

    void a_star(Graph &graph) {
        struct NodeRecord {
            Node* node;
            double f;   // f = g + h
            double g;   // coste acumulado desde src
            bool operator<(NodeRecord const& o) const {
                // invertido para que priority_queue sea min-heap
                return f > o.f;
            }
        };

        // Heurística real (distancia euclidiana)
        auto heuristic = [&](Node* a, Node* b) {
            double dx = a->coord.x - b->coord.x;
            double dy = a->coord.y - b->coord.y;
            return std::sqrt(dx*dx + dy*dy);
        };

        std::unordered_map<Node*, double> gScore;
        std::unordered_map<Node*, Node*>  parent;
        gScore.reserve(graph.nodes.size());
        parent.reserve(graph.nodes.size());

        std::unordered_set<Node*> closed;
        closed.reserve(graph.nodes.size());

        std::priority_queue<NodeRecord> openSet;

        for (auto& [_, n] : graph.nodes) {
            gScore[n] = std::numeric_limits<double>::infinity();
            parent[n] = nullptr;
        }
        gScore[src] = 0.0;
        openSet.push({ src, /*f=*/heuristic(src, dest), /*g=*/0.0 });

        visited_edges.clear();
        path.clear();
        visited_count = 0;

        while (!openSet.empty()) {
            NodeRecord rec = openSet.top(); openSet.pop();
            Node* u = rec.node;

            if (!closed.insert(u).second)
                continue;

            if (u == dest)
                break;

            ++visited_count;

            for (Edge* e : u->edges) {
                Node* v = e->dest;
                double g2 = rec.g + e->length;
                if (g2 < gScore[v]) {
                    gScore[v] = g2;
                    parent[v] = u;
                    double f2 = g2 + heuristic(v, dest);
                    openSet.push({ v, f2, g2 });

                    visited_edges.emplace_back(
                            u->coord,
                            v->coord,
                            sf::Color(200,200,200),
                            0.9f
                    );
                    //render();

                }
            }
        }

        last_parent = parent;
        set_final_path(parent);
    }


    void best_first(Graph &graph) {
        std::unordered_map<Node*, Node*> parent;
        std::unordered_set<Node*>       visited;
        parent.reserve(graph.nodes.size());
        visited.reserve(graph.nodes.size());

        auto cmp = [&](Node* a, Node* b){
            return heuristic(a, dest) > heuristic(b, dest);
        };
        std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

        visited.insert(src);
        pq.push(src);

        while (!pq.empty()) {
            Node* u = pq.top(); pq.pop();

            ++visited_count;

            if (u == dest) break;


            for (Edge* e : u->edges) {
                Node* v = (e->src == u ? e->dest : e->src);
                if (visited.insert(v).second) {
                    parent[v] = u;
                    pq.push(v);
                    visited_edges.emplace_back(u->coord, v->coord, sf::Color(200,200,200), 0.9f);
                }
                //render();

            }
        }
        last_parent = parent;

        set_final_path(parent);
    }



    //* --- render ---
    // En cada iteración de los algoritmos esta función es llamada para dibujar los cambios en el 'window_manager'
    void render() {
        sf::RenderWindow &win = window_manager->get_window();

        for (const sfLine &line : visited_edges) {
            line.draw(win, sf::RenderStates::Default);
        }

        win.display();
    }

    //* --- set_final_path ---
    // Esta función se usa para asignarle un valor a 'this->path' al final de la simulación del algoritmo.
    // 'parent' es un std::unordered_map que recibe un puntero a un vértice y devuelve el vértice anterior a el,
    // formando así el 'path'.
    //
    // ej.
    //     parent(a): b
    //     parent(b): c
    //     parent(c): d
    //     parent(d): NULL
    //
    // Luego, this->path = [Line(a.coord, b.coord), Line(b.coord, c.coord), Line(c.coord, d.coord)]
    //
    // Este path será utilizado para hacer el 'draw()' del 'path' entre 'src' y 'dest'.
    //*
    void set_final_path(std::unordered_map<Node*, Node*>& parent) {
        // Limpia cualquier camino previo
        path.clear();

        // Reconstruye la ruta desde dest hasta src usando el mapa parent
        for (Node* cur = dest; cur != nullptr && parent[cur] != nullptr; cur = parent[cur]) {
            Node* p = parent[cur];
            path.emplace_back(
                    p->coord,
                    cur->coord,
                    sf::Color::Blue,
                    0.6f
            );
        }

        // Actualmente path está al revés (de dest a src), lo invertimos
        std::reverse(path.begin(), path.end());
    }


public:
    Node *src = nullptr;
    Node *dest = nullptr;

    explicit PathFindingManager(WindowManager *window_manager) : window_manager(window_manager) {}

    void exec(Graph &graph, Algorithm algorithm) {
        if (!src || !dest) return;

        // limpia estado previo
        path.clear();
        visited_edges.clear();
        visited_count = 0;
        last_parent.clear();

        // mide tiempo
        auto t0 = std::chrono::high_resolution_clock::now();

        // ejecuta
        switch (algorithm) {
            case Dijkstra:  dijkstra(graph);   break;
            case BFS: best_first(graph); break;
            case AStar:     a_star(graph);     break;
            default:                          return;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double,std::milli>(t1-t0).count();

        // calcula coste total remontándote a last_parent
        double cost = 0.0;
        for (Node* cur = dest; cur != src; cur = last_parent[cur]) {
            Node* p = last_parent[cur];
            // busca arista p->cur
            for (Edge* e : p->edges) {
                if ((e->src==p && e->dest==cur) || (!e->one_way && e->src==cur && e->dest==p)) {
                    cost += e->length;
                    break;
                }
            }
        }

        // muestra resultados
        const char* name = algorithm==Dijkstra ? "Dijkstra"
                                               : algorithm==BFS       ? "Greedy BFS"
                                                                      : "A*";
        std::cout << "=== " << name << " ===\n"
                  << "Tiempo      : " << ms   << " ms\n"
                  << "Coste total : " << cost << "\n"
                  << "Nodos visit.: " << visited_count << "\n\n";
    }


    void reset() {
        path.clear();
        visited_edges.clear();

        if (src) {
            src->reset();
            src = nullptr;
            // ^^^ Pierde la referencia luego de restaurarlo a sus valores por defecto
        }
        if (dest) {
            dest->reset();
            dest = nullptr;
            // ^^^ Pierde la referencia luego de restaurarlo a sus valores por defecto
        }
    }

    void draw(bool draw_extra_lines) {
        // Dibujar todas las aristas visitadas
        if (draw_extra_lines) {
            for (sfLine &line: visited_edges) {
                line.draw(window_manager->get_window(), sf::RenderStates::Default);
            }
        }

        // Dibujar el camino resultante entre 'str' y 'dest'
        for (sfLine &line: path) {
            line.draw(window_manager->get_window(), sf::RenderStates::Default);
        }

        // Dibujar el nodo inicial
        if (src != nullptr) {
            src->draw(window_manager->get_window());
        }

        // Dibujar el nodo final
        if (dest != nullptr) {
            dest->draw(window_manager->get_window());
        }
    }
};


#endif //HOMEWORK_GRAPH_PATH_FINDING_MANAGER_H
