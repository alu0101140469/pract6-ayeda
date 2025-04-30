#ifndef ARBOL_HPP
#define ARBOL_HPP

#include <iostream>
#include <queue>

// Forward declarations
template<typename Key> class AB;
template<typename Key> class ABB;
template<typename Key> class AVL;

// Nodo genérico de árbol binario
template<typename Key>
class NodoB {
protected:
    Key    dato;   // clave almacenada
    NodoB* izdo;   // puntero a hijo izquierdo
    NodoB* dcho;   // puntero a hijo derecho
public:
    NodoB(const Key& k) : dato(k), izdo(nullptr), dcho(nullptr) {}
    virtual ~NodoB() = default;

    // Para que AB, ABB y AVL accedan a izdo/dcho
    template<typename K> friend class AB;
    template<typename K> friend class ABB;
    template<typename K> friend class AVL;
};

// Árbol binario abstracto
template<typename Key>
class AB {
protected:
    NodoB<Key>* raiz = nullptr;

public:
    virtual ~AB() { clear(raiz); }

    // Inserción puro virtual
    virtual bool insertar(const Key& k) = 0;

    // Búsqueda recursiva
    bool buscar(const Key& k) const {
        return buscarRec(raiz, k);
    }

    // Recorrido inorden
    void inorden() const {
        inordenRec(raiz);
        std::cout << "\n";
    }

    // Impresión por niveles
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
                auto n = q.front(); q.pop();
                if (n) {
                    os << "[" << n->dato << "] ";
                    q.push(n->izdo);
                    q.push(n->dcho);
                } else {
                    os << "[.] ";
                }
            }
            os << "\n";
        }
        return os;
    }

protected:
    // Libera memoria en postorden
    static void clear(NodoB<Key>* n) {
        if (!n) return;
        clear(n->izdo);
        clear(n->dcho);
        delete n;
    }

    // Auxiliar búsqueda
    static bool buscarRec(NodoB<Key>* n, const Key& k) {
        if (!n) return false;
        if (k == n->dato) return true;
        if (k < n->dato)  return buscarRec(n->izdo, k);
        else              return buscarRec(n->dcho, k);
    }

    // Auxiliar inorden
    static void inordenRec(NodoB<Key>* n) {
        if (!n) return;
        inordenRec(n->izdo);
        std::cout << n->dato << " ";
        inordenRec(n->dcho);
    }
};

// ABB: árbol binario de búsqueda no balanceado
template<typename Key>
class ABB : public AB<Key> {
public:
    bool insertar(const Key& k) override {
        return insertarRec(this->raiz, k);
    }

private:
    bool insertarRec(NodoB<Key>*& n, const Key& k) {
        if (!n) {
            n = new NodoB<Key>(k);
            return true;
        }
        if (k == n->dato) return false;      // no duplicados
        if (k < n->dato)  return insertarRec(n->izdo, k);
        else              return insertarRec(n->dcho, k);
    }
};

// Nodo especializado para AVL con factor de balance
template<typename Key>
class NodoAVL : public NodoB<Key> {
    int bal;  // factor de balance: altura(izdo) - altura(dcho)
public:
    NodoAVL(const Key& k) : NodoB<Key>(k), bal(0) {}
    template<typename K> friend class AVL;
};

// AVL: ABB balanceado con rotaciones y modo traza
template<typename Key>
class AVL : public ABB<Key> {
    bool traza;

    // Contadores de rotaciones
    int cntII = 0;
    int cntDD = 0;
    int cntID = 0;
    int cntDI = 0;

public:
    AVL(bool tr = false) : traza(tr) {}

    bool insertar(const Key& k) override {
        bool crece = false;
        return insertarRec(reinterpret_cast<NodoAVL<Key>*&>(this->raiz), k, crece);
    }

    // Método público para imprimir con/sin balances
    void print(bool withBal) const {
        if (withBal) 
            printConBal(reinterpret_cast<NodoAVL<Key>*>(this->raiz));
        else
            std::cout << *static_cast<const ABB<Key>*>(this);
    }

private:
    // Calcula la altura de un nodo AVL
    static int altura(NodoAVL<Key>* n) {
        if (!n) return 0;
        return 1 + std::max(
            altura(reinterpret_cast<NodoAVL<Key>*>(n->izdo)),
            altura(reinterpret_cast<NodoAVL<Key>*>(n->dcho))
        );
    }

    // Actualiza factor de balance: izdo - dcho
    static void actualizarBal(NodoAVL<Key>* n) {
        n->bal = altura(reinterpret_cast<NodoAVL<Key>*>(n->izdo))
               - altura(reinterpret_cast<NodoAVL<Key>*>(n->dcho));
    }

    // Rotación II (simple derecha)
    NodoAVL<Key>* rotII(NodoAVL<Key>* r) {
        auto p = reinterpret_cast<NodoAVL<Key>*>(r->izdo);
        r->izdo = p->dcho; p->dcho = r;
        actualizarBal(r); actualizarBal(p);
        // Contar sólo si r->dato es par
        if ((long)r->dato % 2 == 0) cntII++;
        return p;
    }

    // Rotación DD (simple izquierda)
    NodoAVL<Key>* rotDD(NodoAVL<Key>* r) {
        auto p = reinterpret_cast<NodoAVL<Key>*>(r->dcho);
        r->dcho = p->izdo; p->izdo = r;
        actualizarBal(r); actualizarBal(p);
        // Contar sólo si r->dato es par
        if ((long)r->dato % 2 == 0) cntDD++;
        return p;
    }

    // Rotación ID (doble: izquierda-derecha)
    NodoAVL<Key>* rotID(NodoAVL<Key>* r) {
        // primera mitad: DD sobre hijo izquierdo
        r->izdo = rotDD(reinterpret_cast<NodoAVL<Key>*>(r->izdo));
        // contar sólo si r->dato es impar
        if ((long)r->dato % 2 != 0) cntID++;
        // segunda mitad: II sobre r
        return rotII(r);
    }

    // Rotación DI (doble: derecha-izquierda)
    NodoAVL<Key>* rotDI(NodoAVL<Key>* r) {
        // primera mitad: II sobre hijo derecho
        r->dcho = rotII(reinterpret_cast<NodoAVL<Key>*>(r->dcho));
        // contar sólo si r->dato es impar
        if ((long)r->dato % 2 != 0) cntDI++;
        // segunda mitad: DD sobre r
        return rotDD(r);
    }

    // Imprime el árbol por niveles mostrando (bal) en cada nodo
    void printConBal(NodoAVL<Key>* root) const {
        if (!root) {
            std::cout << "Árbol vacío\nNivel 0: [.]\n";
            return;
        }
        std::queue<NodoAVL<Key>*> q;
        q.push(root);
        int nivel = 0;
        while (!q.empty()) {
            int sz = q.size();
            std::cout << "Nivel " << nivel++ << ": ";
            for (int i = 0; i < sz; ++i) {
                auto n = q.front(); q.pop();
                if (n) {
                    std::cout << "[" << n->dato << "(" << n->bal << ")] ";
                    q.push(reinterpret_cast<NodoAVL<Key>*>(n->izdo));
                    q.push(reinterpret_cast<NodoAVL<Key>*>(n->dcho));
                } else {
                    std::cout << "[.] ";
                }
            }
            std::cout << "\n";
        }
    }

    // Inserción recursiva con rebalanceo y conteo
    bool insertarRec(NodoAVL<Key>*& n, const Key& k, bool& crece) {
        if (!n) {
            n = new NodoAVL<Key>(k);
            crece = true;
            return true;
        }
        if (k == n->dato) {
            crece = false;
            return false;
        }
        bool res;
        if (k < n->dato) {
            res = insertarRec(reinterpret_cast<NodoAVL<Key>*&>(n->izdo), k, crece);
            if (crece) n->bal++;  // izquierdo crece → bal aumenta
        } else {
            res = insertarRec(reinterpret_cast<NodoAVL<Key>*&>(n->dcho), k, crece);
            if (crece) n->bal--;  // derecho crece → bal disminuye
        }
        if (!res) return false;

        // Detectar y corregir desbalance
        if (n->bal == 2 || n->bal == -2) {
            if (traza) {
                std::cout << "Desbalanceo:\n";
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
                std::cout << "Contadores → "
                          << "II: " << cntII << "   "
                          << "ID: " << cntID << "   "
                          << "DI: " << cntDI << "   "
                          << "DD: " << cntDD << "\n\n";
            }
            crece = false;
        }
        return true;
    }
};

#endif // ARBOL_HPP
