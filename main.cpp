#include <iostream>
#include "sfml_char_traits_fix.hpp"
#include "gui.h"

using namespace std;

int main() {
    size_t id_src, id_dst;

    cout << "Ingrese ID de nodo origen: ";
    if (!(cin >> id_src)) {
        cerr << "Error leyendo ID de nodo origen\n";
        return 1;
    }

    cout << "Ingrese ID de nodo destino: ";
    if (!(cin >> id_dst)) {
        cerr << "Error leyendo ID de nodo destino\n";
        return 1;
    }

    GUI gui("nodes.csv", "edges.csv");
    gui.setStartEnd(id_src, id_dst);
    gui.main_loop();

    return 0;
}
