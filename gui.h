// Created by juan-diego on 3/11/24.

#ifndef HOMEWORK_GRAPH_GUI_H
#define HOMEWORK_GRAPH_GUI_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <limits>
#include <map>

#include "window_manager.h"
#include "path_finding_manager.h"

class GUI {
    WindowManager       window_manager;
    PathFindingManager  path_finding_manager;
    Graph               graph;

    static Node* _1NN(const std::map<std::size_t, Node*>& nodes, sf::Vector2f pos) {
        Node* nearest = nullptr;
        float  minDist = std::numeric_limits<float>::infinity();
        for (auto& [_, node] : nodes) {
            float dx = node->coord.x - pos.x;
            float dy = node->coord.y - pos.y;
            float d  = std::sqrt(dx*dx + dy*dy);
            if (d < minDist) {
                minDist = d;
                nearest = node;
            }
        }
        return nearest;
    }

public:
    explicit GUI(const std::string& nodes_path, const std::string& edges_path)
            : window_manager(600, 800),
              path_finding_manager(&window_manager),
              graph(&window_manager)
    {
        graph.parse_csv(nodes_path, edges_path);
        window_manager.get_window().setFramerateLimit(200);
    }

    void setStartEnd(size_t src_id, size_t dst_id) {
        auto it_s = graph.nodes.find(src_id);
        auto it_d = graph.nodes.find(dst_id);
        if (it_s != graph.nodes.end() && it_d != graph.nodes.end()) {
            path_finding_manager.src  = it_s->second;
            path_finding_manager.dest = it_d->second;
            // Resalta visualmente
            path_finding_manager.src->color  = sf::Color::Green;
            path_finding_manager.dest->color = sf::Color::Cyan;
            path_finding_manager.src->radius =
            path_finding_manager.dest->radius = 5.0f;
        } else {
            std::cerr << "setStartEnd: IDs inválidos ("
                      << src_id << ", " << dst_id << ")\n";
        }
    }

    void main_loop() {
        bool draw_extra_lines = false;

        while (window_manager.is_open()) {
            sf::Event event;
            while (window_manager.poll_event(event)) {
                switch (event.type) {
                    // Cerrar ventana
                    case sf::Event::Closed:
                        window_manager.close();
                        break;

                        // Ejecución de algoritmos y toggles
                    case sf::Event::KeyPressed: {
                        switch (event.key.code) {
                            case sf::Keyboard::D:
                                path_finding_manager.exec(graph, Dijkstra);
                                break;
                            case sf::Keyboard::A:
                                path_finding_manager.exec(graph, AStar);
                                break;
                            case sf::Keyboard::B:
                                path_finding_manager.exec(graph, BFS);
                                break;
                            case sf::Keyboard::R:
                                path_finding_manager.reset();
                                break;
                            case sf::Keyboard::E:
                                draw_extra_lines = !draw_extra_lines;
                                break;
                            case sf::Keyboard::Q:
                                window_manager.close();
                                break;
                            case sf::Keyboard::Left: {
                                sf::View v = window_manager.getView();
                                v.move(-50.f, 0.f);
                                window_manager.setView(v);
                                break;
                            }
                            case sf::Keyboard::Right: {
                                sf::View v = window_manager.getView();
                                v.move(50.f, 0.f);
                                window_manager.setView(v);
                                break;
                            }
                            case sf::Keyboard::Up: {
                                sf::View v = window_manager.getView();
                                v.move(0.f, -50.f);
                                window_manager.setView(v);
                                break;
                            }
                            case sf::Keyboard::Down: {
                                sf::View v = window_manager.getView();
                                v.move(0.f, 50.f);
                                window_manager.setView(v);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }

                    case sf::Event::MouseWheelScrolled:
                        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                            float factor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
                            sf::View v = window_manager.getView();
                            v.zoom(factor);
                            window_manager.setView(v);
                        }
                        break;

                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            auto& win = window_manager.get_window();
                            sf::Vector2i pix{ event.mouseButton.x, event.mouseButton.y };
                            sf::Vector2f worldPos = win.mapPixelToCoords(pix, window_manager.getView());

                            const float screenRadius = 8.f;
                            auto winSize = win.getSize();
                            float pixelsPerWorld = float(winSize.y) / window_manager.getView().getSize().y;
                            float worldRadius     = screenRadius / pixelsPerWorld;

                            Node* n = _1NN(graph.nodes, worldPos);

                            if (n) {
                                if (path_finding_manager.src == nullptr) {
                                    path_finding_manager.src = n;
                                    n->color  = sf::Color::Green;
                                    n->radius = worldRadius;
                                }
                                else if (path_finding_manager.dest == nullptr) {
                                    path_finding_manager.dest = n;
                                    n->color  = sf::Color::Cyan;
                                    n->radius = worldRadius;
                                }
                            }
                        }
                        break;

                    default:
                        break;
                }
            }

            window_manager.clear();
            window_manager.get_window().setView(window_manager.getView());
            graph.draw();
            path_finding_manager.draw(draw_extra_lines);
            window_manager.display();
        }
    }
};

#endif // HOMEWORK_GRAPH_GUI_H
