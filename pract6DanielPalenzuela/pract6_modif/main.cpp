#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "nif.hpp"
#include "Arbol.hpp"

// Daniel Palenzuela Álvarez alu0101140469
// Ejemplos pdf:
// ./arbol -ab abb -init manual -trace n
// ./arbol -ab avl -init manual -trace n
// ./arbol -ab avl -init manual -trace y

// Modif: 4 contadores, uno para cada rotacion (ii, di, id, dd), 
// cuando es ii o dd solo contaremos cuando el nodo en el que se efectua 
// es par. Cuando di y id cuando el nodo sea impar. 
// Hay que mostrarlo en cada iteracion de la traza, 
// debajo del arbol en la ejecucion.

// Muestra la ayuda
void printUsage(const char* prog) {
    std::cout << "Uso: " << prog << " [--help] -ab <abb|avl> -init <manual|random|file> [s] [f] -trace <y|n>\n"
              << "Opciones:\n"
              << "  --help                Muestra este mensaje\n"
              << "  -ab <abb|avl>         Tipo de árbol a usar\n"
              << "  -init manual          Árbol vacío, inserción manual\n"
              << "  -init random <s>      Genera s números aleatorios\n"
              << "  -init file <s> <f>    Lee s números de fichero f\n"
              << "  -trace <y|n>          Activa modo traza (solo para AVL)\n";
}

int main(int argc, char* argv[]) {
    // Comando --help
    if (argc == 2 && std::string(argv[1])=="--help") {
        printUsage(argv[0]);
        return 0;
    }
    // mínimo de argumentos
    if (argc < 7) {
        printUsage(argv[0]);
        return 1;
    }

    // Parseo básico de opciones
    std::string abType, initType, traceOpt;
    int s = 0; // número de elementos
    std::string fname; // nombre de fichero si se requiere
    for(int i=1;i<argc;i++){
        std::string a = argv[i];
        if (a=="-ab" && i+1<argc) abType=argv[++i];
        else if (a=="-init" && i+1<argc) {
            initType=argv[++i];
            if (initType=="random" && i+1<argc) s=std::atoi(argv[++i]);
            else if (initType=="file" && i+2<argc) {
                s=std::atoi(argv[++i]);
                fname=argv[++i];
            }
        }
        else if (a=="-trace" && i+1<argc) traceOpt = argv[++i];
    }
    bool trace = (traceOpt=="y"); // solo para avl

    // Generar o leer datos en vector<nif>
    std::vector<nif> datos;
    if (initType == "manual") {
        std::cout << "Inicializando en modo manual: árbol vacío.\n";
    }
    else if (initType=="random") {
        srand(time(nullptr));
        for (int i=0;i<s;i++) datos.emplace_back(); // constructor aleatorio
    }
    else if (initType=="file") {
        std::ifstream fi(fname);
        if (!fi) { std::cerr<<"No puedo abrir "<<fname<<"\n"; return 1; }
        long x;
        for (int i=0;i<s && fi>>x;i++) datos.emplace_back(x);
    }

    // Selección de árbol
    if (abType=="abb") {
        ABB<nif> tree; // ABB sin traza
        for (auto& k: datos) tree.insertar(k); // inserción inicial
        std::cout<<"Árbol inicial:\n"<<tree; // muestra nivel por nivel

        // Menú interactivo
        int op;
        do {
            std::cout<<"[0] Salir\n [1] Insertar clave\n [2] Buscar clave\n [3] Mostrar árbol inorden\n > ";
            std::cin>>op;
            if (op==1) {
                long v; std::cout<<"Clave: "; std::cin>>v;
                if (tree.insertar(nif(v))) std::cout<<tree;
                else std::cout<<"Ya existe\n";
            } else if (op==2) {
                long v; std::cout<<"Clave: "; std::cin>>v;
                std::cout<<(tree.buscar(nif(v))?"Encontrada\n":"No encontrada\n");
            } else if (op==3) {
                tree.inorden();
            }
        } while(op!=0);
    }
    else if (abType=="avl") {
        AVL<nif> tree(trace); // AVL con/sin traza
        for (auto& k: datos) tree.insertar(k);
        std::cout<<"Árbol inicial:\n";
        tree.print(trace);
        tree.printCont(); // modif

        int op;
        do {
            std::cout<<"[0] Salir\n [1] Insertar clave\n [2] Buscar clave\n [3] Mostrar árbol inorden\n > ";
            std::cin>>op;
            if (op==1) {
                long v; std::cout<<"Clave: "; std::cin>>v;
                if (tree.insertar(nif(v))) {
                    tree.print(trace);
                    tree.printCont(); // modif
                } else std::cout<<"Ya existe\n";
            } else if (op==2) {
                long v; std::cout<<"Clave: "; std::cin>>v;
                std::cout<<(tree.buscar(nif(v))?"Encontrada\n":"No encontrada\n");
            } else if (op==3) {
                tree.inorden();
            }
        } while(op!=0);
    }
    else {
        std::cerr<<"Tipo de árbol no reconocido\n";
        return 1;
    }

    return 0;
}