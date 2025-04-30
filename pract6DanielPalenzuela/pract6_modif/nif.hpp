#ifndef NIF_HPP
#define NIF_HPP

#include <cstdlib>
#include <iostream>

// Daniel Palenzuela Álvarez alu0101140469

// Clase nif: representa un número de 8 dígitos y define sobrecargas para comparación e impresión.
class nif {
private:
    long number;  // Almacena el número (8 dígitos)
public:
    // Constructor por defecto, que genera aleatoriamente un número de 8 dígitos.
    nif() {
        number = 10000000 + rand() % 90000000;
    }
    // Constructor que inicializa el objeto con el número dado.
    nif(long n) : number(n) {}

    // Conversión al tipo long, que permite usar objetos nif en operaciones numéricas.
    operator long() const { return number; }

    // Sobrecarga del operador de igualdad
    bool operator==(const nif &other) const { return number == other.number; }
    // Sobrecarga del operador de desigualdad
    bool operator!=(const nif &other) const { return number != other.number; }
    // Sobrecarga del operador menor que
    bool operator<(const nif &other) const { return number < other.number; }
    // Sobrecarga del operador mayor que
    bool operator>(const nif &other) const { return number > other.number; }

    // Sobrecarga del operador de salida para imprimir directamente el número
    friend std::ostream& operator<<(std::ostream &os, const nif &n) {
        os << n.number;
        return os;
    }
};

#endif