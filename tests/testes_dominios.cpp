#include "testes_dominios.h"

int TUDominio::run() {
    setUp();
    testarCenarioSucesso();
    testarCenarioFalha();
    tearDown();
    return estado;
}

// --- Email ---
void TUEmail::setUp() { dominio = new Email(); estado = SUCESSO; }
void TUEmail::tearDown() { delete dominio; }
void TUEmail::testarCenarioSucesso() {
    try {
        dominio->setValor("usuario.teste-1@unb.br");
        if (dominio->getValor() != "usuario.teste-1@unb.br") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUEmail::testarCenarioFalha() {
    try {
        dominio->setValor(".invalido@unb.br");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Estado ---
void TUEstado::setUp() { dominio = new Estado(); estado = SUCESSO; }
void TUEstado::tearDown() { delete dominio; }
void TUEstado::testarCenarioSucesso() {
    try {
        dominio->setValor("FAZENDO");
        if (dominio->getValor() != "FAZENDO") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUEstado::testarCenarioFalha() {
    try {
        dominio->setValor("CONCLUIDO");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Identificador ---
void TUIdentificador::setUp() { dominio = new Identificador(); estado = SUCESSO; }
void TUIdentificador::tearDown() { delete dominio; }
void TUIdentificador::testarCenarioSucesso() {
    try {
        dominio->setValor("ABC123");
        if (dominio->getValor() != "ABC123") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUIdentificador::testarCenarioFalha() {
    try {
        dominio->setValor("123ABC");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Limite ---
void TULimite::setUp() { dominio = new Limite(); estado = SUCESSO; }
void TULimite::tearDown() { delete dominio; }
void TULimite::testarCenarioSucesso() {
    try {
        dominio->setValor(10);
        if (dominio->getValor() != 10) estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TULimite::testarCenarioFalha() {
    try {
        dominio->setValor(30);
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Nome ---
void TUNome::setUp() { dominio = new Nome(); estado = SUCESSO; }
void TUNome::tearDown() { delete dominio; }
void TUNome::testarCenarioSucesso() {
    try {
        dominio->setValor("Joao Silva");
        if (dominio->getValor() != "Joao Silva") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUNome::testarCenarioFalha() {
    try {
        dominio->setValor("Joao  Silva");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Papel ---
void TUPapel::setUp() { dominio = new Papel(); estado = SUCESSO; }
void TUPapel::tearDown() { delete dominio; }
void TUPapel::testarCenarioSucesso() {
    try {
        dominio->setValor("GESTOR");
        if (dominio->getValor() != "GESTOR") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUPapel::testarCenarioFalha() {
    try {
        dominio->setValor("ADMIN");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Prioridade ---
void TUPrioridade::setUp() { dominio = new Prioridade(); estado = SUCESSO; }
void TUPrioridade::tearDown() { delete dominio; }
void TUPrioridade::testarCenarioSucesso() {
    try {
        dominio->setValor("ALTA");
        if (dominio->getValor() != "ALTA") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUPrioridade::testarCenarioFalha() {
    try {
        dominio->setValor("URGENTE");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Senha ---
void TUSenha::setUp() { dominio = new Senha(); estado = SUCESSO; }
void TUSenha::tearDown() { delete dominio; }
void TUSenha::testarCenarioSucesso() {
    try {
        dominio->setValor("Ab12c");
        if (dominio->getValor() != "Ab12c") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUSenha::testarCenarioFalha() {
    try {
        dominio->setValor("abcde");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Tamanho ---
void TUTamanho::setUp() { dominio = new Tamanho(); estado = SUCESSO; }
void TUTamanho::tearDown() { delete dominio; }
void TUTamanho::testarCenarioSucesso() {
    try {
        dominio->setValor("GRANDE");
        if (dominio->getValor() != "GRANDE") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUTamanho::testarCenarioFalha() {
    try {
        dominio->setValor("GIGANTE");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Texto ---
void TUTexto::setUp() { dominio = new Texto(); estado = SUCESSO; }
void TUTexto::tearDown() { delete dominio; }
void TUTexto::testarCenarioSucesso() {
    try {
        dominio->setValor("Tarefa inicial.");
        if (dominio->getValor() != "Tarefa inicial.") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUTexto::testarCenarioFalha() {
    try {
        dominio->setValor("tarefa sem ponto");
        estado = FALHA;
    } catch (invalid_argument&) {}
}

// --- Timestamp ---
void TUTimestamp::setUp() { dominio = new Timestamp(); estado = SUCESSO; }
void TUTimestamp::tearDown() { delete dominio; }
void TUTimestamp::testarCenarioSucesso() {
    try {
        dominio->setValor("29-FEV-2024-14:30"); // Ano bissexto valido
        if (dominio->getValor() != "29-FEV-2024-14:30") estado = FALHA;
    } catch (invalid_argument&) { estado = FALHA; }
}
void TUTimestamp::testarCenarioFalha() {
    try {
        dominio->setValor("29-FEV-2023-14:30"); // 2023 nao e bissexto
        estado = FALHA;
    } catch (invalid_argument&) {}
}
