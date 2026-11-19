#include "tests/testes_servicos.h"

// Executavel que roda todos os testes de unidade da camada de servico.
int main() {
    TesteServicos testes;
    return testes.executar() == 0 ? 0 : 1;
}
