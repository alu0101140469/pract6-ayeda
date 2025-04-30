#ifndef ARBOL_HPP
#define ARBOL_HPP

#include <iostream>
#include <queue>

// Daniel Palenzuela Álvarez alu0101140469

template<typename Key> class AB;
template<typename Key> class ABB;
template<typename Key> class AVL;

// Nodo genérico de árbol binario
template<typename Key>
class NodoB {
protected:
    Key dato; // clave almacenada en el nodo
    NodoB* izdo; // puntero a hijo izquierdo
    NodoB* dcho; // puntero a hijo derecho
public:
    // Constructor que inicializa dato y pone hijos a nullptr
    NodoB(const Key& k) : dato(k), izdo(nullptr), dcho(nullptr) {}
    virtual ~NodoB() = default;

    // permitimos que AB, ABB, AVL accedan a izdo/dcho
    template<typename K> friend class AB;
    template<typename K> friend class ABB;
    template<typename K> friend class AVL;
};

// Árbol binario abstracto
// Proporciona buscar, inorden y salida por niveles
template<typename Key>
class AB {
protected:
    NodoB<Key>* raiz; // puntero a la raíz
public:
    AB() : raiz(nullptr) {}
    virtual ~AB() { clear(raiz); }

    // Inserta clave y devuelve true si tuvo éxito
    virtual bool insertar(const Key& k) = 0;

    // Busca clave
    bool buscar(const Key& k) const {
        return buscarRec(raiz, k);
    }

    // Recorre e imprime inorden (izdo-raíz-dcho)
    void inorden() const {
        inordenRec(raiz);
        std::cout << "\n";
    }

    // Sobrecarga de << para mostrar árbol por niveles
    friend std::ostream& operator<<(std::ostream& os, const AB& ab) {
        if (!ab.raiz) {
            os << "Árbol vacío\nNivel 0: [.]\n";
            return os;
        }
        std::queue<NodoB<Key>*> q;
        q.push(ab.raiz);
        int nivel = 0;
        while (!q.empty()) {
            int sz = q.size();
            os << "Nivel " << nivel++ << ": ";
            for (int i = 0; i < sz; ++i) {
                NodoB<Key>* nodo = q.front(); q.pop();
                if (nodo) {
                    os << "[" << nodo->dato << "]";
                    q.push(nodo->izdo);
                    q.push(nodo->dcho);
                } else {
                    os << "[.]";
                }
                os << " ";
            }
            os << "\n";
        }
        return os;
    }

protected:
    // Libera toda la memoria
    static void clear(NodoB<Key>* n) {
        if (!n) return;
        clear(n->izdo);
        clear(n->dcho);
        delete n;
    }

    // Recursivo para buscar
    static bool buscarRec(NodoB<Key>* n, const Key& k) {
        if (!n) return false;
        if (k == n->dato) return true;
        if (k < n->dato) return buscarRec(n->izdo, k);
        else return buscarRec(n->dcho, k);
    }

    // Recursivo inorden
    static void inordenRec(NodoB<Key>* n) {
        if (!n) return;
        inordenRec(n->izdo);
        std::cout << n->dato << " ";
        inordenRec(n->dcho);
    }
};

// ABB: hereda de AB
template<typename Key>
class ABB : public AB<Key> {
public:
    bool insertar(const Key& k) override {
        return insertarRec(this->raiz, k);
    }
private:
// Inserción recursiva y rechaza duplicados
    bool insertarRec(NodoB<Key>*& n, const Key& k) {
        if (!n) {
            n = new NodoB<Key>(k);
            return true;
        }
        if (k == n->dato) return false; // no repetidos
        if (k < n->dato) return insertarRec(n->izdo, k);
        else return insertarRec(n->dcho, k);
    }
};

// Nodo para AVL (añade factor de balanceo)
template<typename Key>
class NodoAVL : public NodoB<Key> {
    int bal; // balance = altura(izdo) - altura(dcho)
public:
    NodoAVL(const Key& k) : NodoB<Key>(k), bal(0) {}
    template<typename K> friend class AVL;
};

// AVL: hereda de ABB
template<typename Key>
class AVL : public ABB<Key> {
    bool traza; // si true, muestra pasos de rotación

    // modif: contadores
    int contII = 0;
    int contDD = 0;
    int contID = 0;
    int contDI = 0;
public:
    AVL(bool tr = false) : traza(tr) {}
    bool insertar(const Key& k) override {
        bool crece = false;
        // reinterpret_cast para convertir NodoB<Key>* a NodoAVL<Key>*
        return insertarRec(reinterpret_cast<NodoAVL<Key>*&>(this->raiz), k, crece);
    }

    void print(bool withBal) const {
        if (withBal)  {
            printConBal(reinterpret_cast<NodoAVL<Key>*>(this->raiz));
        } else
            std::cout << *static_cast<const ABB<Key>*>(this);
    }

    // modif
    void printCont() const {
        // Muestra los contadores acumulados
        std::cout << "Contadores: \n"
        << "II: " << contII << "   "
        << "ID: " << contID << "   "
        << "DI: " << contDI << "   "
        << "DD: " << contDD << "  \n";
    }

private:
    // Calcula altura de un nodo AVL recursivamente
    static int altura(NodoAVL<Key>* n) {
        if (!n) return 0;
        return 1 + std::max(altura(reinterpret_cast<NodoAVL<Key>*>(n->izdo)),
                            altura(reinterpret_cast<NodoAVL<Key>*>(n->dcho)));
    }

    // Actualiza el factor de balance de n
    static void actualizarBal(NodoAVL<Key>* n) {
        n->bal = altura(reinterpret_cast<NodoAVL<Key>*>(n->izdo))
        - altura(reinterpret_cast<NodoAVL<Key>*>(n->dcho));
    }

    // Rotaciones básicas
    NodoAVL<Key>* rotII(NodoAVL<Key>* r) {
        NodoAVL<Key>* p = reinterpret_cast<NodoAVL<Key>*>(r->izdo);
        r->izdo = p->dcho;
        p->dcho = r;
        actualizarBal(r);
        actualizarBal(p);

        // Contar sólo si r->dato es par
        if ((long)r->dato % 2 == 0) contII++;
        return p;
    }
    NodoAVL<Key>* rotDD(NodoAVL<Key>* r) {
        NodoAVL<Key>* p = reinterpret_cast<NodoAVL<Key>*>(r->dcho);
        r->dcho = p->izdo;
        p->izdo = r;
        actualizarBal(r);
        actualizarBal(p);

        // Contar sólo si r->dato es par
        if ((long)r->dato % 2 == 0) contDD++;
        return p;
    }
    NodoAVL<Key>* rotID(NodoAVL<Key>* r) {
        r->izdo = rotDD(reinterpret_cast<NodoAVL<Key>*>(r->izdo));

        // contar sólo si r->dato es impar
        if ((long)r->dato % 2 != 0) contID++;
        return rotII(r);
    }
    NodoAVL<Key>* rotDI(NodoAVL<Key>* r) {
        r->dcho = rotII(reinterpret_cast<NodoAVL<Key>*>(r->dcho));

        // contar sólo si r->dato es impar
        if ((long)r->dato % 2 != 0) contDI++;
        return rotDD(r);
    }

    // Muestra árbol por niveles incluyendo bal en cada nodo
    void printConBal(NodoAVL<Key>* root) const {
        if (!root) { std::cout<<"Árbol vacío\nNivel 0: [.]\n"; return; }
        std::queue<NodoAVL<Key>*> q;
        q.push(root);
        int nivel = 0;
        while (!q.empty()) {
            int sz = q.size();
            std::cout<<"Nivel "<<nivel++<<": ";
            for (int i = 0; i < sz; ++i) {
                NodoAVL<Key>* n = q.front(); q.pop();
                if (n) {
                    std::cout<<"["<<n->dato<<"("<<n->bal<<")] ";
                    q.push(reinterpret_cast<NodoAVL<Key>*>(n->izdo));
                    q.push(reinterpret_cast<NodoAVL<Key>*>(n->dcho));

                } else std::cout<<"[.] ";
            }
            std::cout<<"\n";
        }
    }

    // Inserción recursiva con control de crecimiento y rebalanceo
    bool insertarRec(NodoAVL<Key>*& n, const Key& k, bool& crece) {
        if (!n) {
            n = new NodoAVL<Key>(k);
            crece = true;
            return true;
        }
        if (k == n->dato) { crece = false; return false; } // duplicado
        bool res;
        if (k < n->dato) {
            res = insertarRec(reinterpret_cast<NodoAVL<Key>*&>(n->izdo), k, crece);
            if (crece) n->bal++; // izquierda creció entonces balance--
        } else {
            res = insertarRec(reinterpret_cast<NodoAVL<Key>*&>(n->dcho), k, crece);
            if (crece) n->bal--; // derecha creció entonces balance++
        }
        if (!res) return false;

        // Si desbalance absoluto == 2, rotar
        if (n->bal == 2 || n->bal == -2) {
            if (traza) {
                std::cout<<"Desbalanceo:\n";
                printConBal(reinterpret_cast<NodoAVL<Key>*>(this->raiz));
            }
            if (n->bal == 2) {
                // caso II o ID
                if (reinterpret_cast<NodoAVL<Key>*>(n->izdo)->bal >= 0) {
                    if (traza) std::cout << "Rotación II en [" << n->dato << "(" << n->bal << ")]:\n";
                    n = rotII(n);
                } else {
                    if (traza) std::cout << "Rotación ID en [" << n->dato << "(" << n->bal << ")]:\n";
                    n = rotID(n);
                }
            } else {
                // caso DD o DI
                if (reinterpret_cast<NodoAVL<Key>*>(n->dcho)->bal <= 0) {
                    if (traza) std::cout << "Rotación DD en [" << n->dato << "(" << n->bal << ")]:\n";
                    n = rotDD(n);
                } else {
                    if (traza) std::cout << "Rotación DI en [" << n->dato << "(" << n->bal << ")]:\n";
                    n = rotDI(n);
                }
            }
            if (traza) {
                // Imprime el árbol tras la rotación
                printConBal(reinterpret_cast<NodoAVL<Key>*>(this->raiz));
                // Muestra los contadores acumulados
                std::cout << "Contadores: \n"
                          << "II: " << contII << "   "
                          << "ID: " << contID << "   "
                          << "DI: " << contDI << "   "
                          << "DD: " << contDD << "  ";
            }
            crece = false; // tras rotación, la altura no crece más aquí  
        }
        return true;
    }
};

#endif