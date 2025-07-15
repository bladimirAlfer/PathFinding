# Tarea de Grafos

## Integrantes: 
- 1 Bladimir Alferez Vicente
  
## Objetivo: 
El objetivo de esta tarea es implementar un **Path Finder** para la ciudad de Lima. 

<p align="center">
    <img src=https://github.com/utec-cs-aed/homework_graph/assets/79115974/b63f69db-17eb-417a-8aa1-8483d8dcdaf0 / >
</p>

## Dependencias

Para esta tarea se solicita utilizar ```C++17``` y la librería ```SFML 2.5```

- Para instalar ```SFML 2.5```:

    - [Windows](https://www.youtube.com/watch?v=HkPRG0vfObc)
    - [MacOS y Linux](https://www.youtube.com/playlist?list=PLvv0ScY6vfd95GMoMe2zc4ZgGxWYj3vua)

Cuando se instale la librería, probar que las siguientes líneas del ```CMakeLists.txt``` encuentren la librería adecuadamente.
```cmake
find_package(SFML 2.5 COMPONENTS graphics window REQUIRED)
if(SFML_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window)
else()
    message("SFML not found")
endif()
```

## Dataset
El dataset consiste de dos csv:

- *nodes.csv*

    ![image](https://github.com/utec-cs-aed/homework_graph/assets/79115974/6a68cf06-196a-4605-83a7-3183e9a3f0ec)


- *edges.csv*

    ![image](https://github.com/utec-cs-aed/homework_graph/assets/79115974/247bbbd7-6203-45f4-8196-fcb0434b0f1d)


## Algoritmos
Se les solicita implementar tres algoritmos para busqueda en grafos

- *Dijkstra*

- *Best First Search*

- *A**

- **Dijkstra**
  - **Complejidad:** `O((|V| + |E|) log |V|)`
  - **Por qué:**
    - Usamos `std::unordered_map<Node*, double> dist` para almacenar las distancias.  
    - Cada vez que encontramos un camino mejor, actualizamos `dist[v]`, su padre y empujamos a la cola de prioridad.  
    - Las operaciones de inserción y extracción en la cola cuestan `O(log |V|)`.  
    - El acceso y la actualización de distancias es `O(1)` promedio gracias a `std::unordered_map`.  
    - Garantiza el coste mínimo, pero explora muchos vértices incluso lejanos al destino.

- **Best First Search**
  - **Complejidad:** `O((|V| + |E|) log |V|)`
  - **Por qué:**
    - Prioriza solo `h(n)` sin acumular `g(n)`, pero la cola de prioridad sigue costando `O(log |V|)`.  
    - Expande muy pocos nodos cercanos a la línea recta al destino, pero **no** garantiza la ruta más corta.  
    - No almacenamos costes acumulados; marcamos visitados con `std::unordered_set<Node*>`.  
    - Al alcanzar el destino, reconstruimos el recorrido usando el mapa de padres.

- **A\***
  - **Complejidad:** `O((|V| + |E|) log |V|)`
  - **Por qué:**
    - Igual que Dijkstra, pero ordena por `f(n) = g(n) + h(n)`.  
    - En el código calculamos `h(n)` como la distancia euclidiana entre el nodo y el destino.  
    - Si la heurística es admisible y consistente, sigue siendo óptimo.  
    - En la práctica reduce drásticamente el número de expansiones alrededor de la ruta directa.


- **Debe realizar un pequeño video (2 min) mostrando la funcionalidad visual de cada algoritmo**

- (video) https://drive.google.com/file/d/1cE5_GekfsoEOxPb9sUj4YgzQ_nabfme4/view?usp=sharing 
  
## Diagrama de clases UML 

![image](https://github.com/utec-cs-aed/homework_graph/assets/79115974/f5a3d89e-cb48-4715-b172-a17e6e27ee24)
