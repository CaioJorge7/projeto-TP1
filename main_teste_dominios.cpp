#include <iostream>
#include "tests/testes_dominios.h"

using namespace std;

int main() {
    cout << "=== EXECUTANDO TESTES UNITARIOS DOS DOMINIOS ===" << endl;

    TUEmail tuEmail;
    cout << "Email: " << (tuEmail.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUEstado tuEstado;
    cout << "Estado: " << (tuEstado.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUIdentificador tuIdentificador;
    cout << "Identificador: " << (tuIdentificador.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TULimite tuLimite;
    cout << "Limite: " << (tuLimite.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUNome tuNome;
    cout << "Nome: " << (tuNome.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUPapel tuPapel;
    cout << "Papel: " << (tuPapel.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUPrioridade tuPrioridade;
    cout << "Prioridade: " << (tuPrioridade.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUSenha tuSenha;
    cout << "Senha: " << (tuSenha.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUTamanho tuTamanho;
    cout << "Tamanho: " << (tuTamanho.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUTexto tuTexto;
    cout << "Texto: " << (tuTexto.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    TUTimestamp tuTimestamp;
    cout << "Timestamp: " << (tuTimestamp.run() == TUDominio::SUCESSO ? "SUCESSO" : "FALHA") << endl;

    return 0;
}
