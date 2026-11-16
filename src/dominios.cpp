#include "dominios.h"
#include <cctype>
#include <vector>
#include <algorithm>
#include <sstream>

void Dominio::setValor(string valor) {
    validar(valor);
    this->valor = valor;
}

// ============================================================================
// EMAIL
// ============================================================================
void Email::validar(string valor) {
    size_t posArroba = valor.find('@');
    if (posArroba == string::npos || valor.find('@', posArroba + 1) != string::npos) {
        throw invalid_argument("Email deve conter exatamente um '@'.");
    }

    string parteLocal = valor.substr(0, posArroba);
    string dominio = valor.substr(posArroba + 1);

    // Validação da parte local
    if (parteLocal.empty() || parteLocal.length() > 64) {
        throw invalid_argument("Parte local do email deve ter entre 1 e 64 caracteres.");
    }
    if (parteLocal.front() == '.' || parteLocal.front() == '-' ||
        parteLocal.back() == '.' || parteLocal.back() == '-') {
        throw invalid_argument("Parte local nao pode iniciar ou terminar com ponto ou hifen.");
    }
    for (size_t i = 0; i < parteLocal.length(); ++i) {
        char c = parteLocal[i];
        bool ehLetraMin = (c >= 'a' && c <= 'z');
        bool ehDigito = (c >= '0' && c <= '9');
        bool ehPontoOuHifen = (c == '.' || c == '-');

        if (!ehLetraMin && !ehDigito && !ehPontoOuHifen) {
            throw invalid_argument("Caractere invalido na parte local do email.");
        }
        if (ehPontoOuHifen) {
            char prox = parteLocal[i + 1];
            if (!((prox >= 'a' && prox <= 'z') || (prox >= '0' && prox <= '9'))) {
                throw invalid_argument("Ponto ou hifen na parte local deve ser seguido por letra ou digito.");
            }
        }
    }

    // Validação do domínio
    if (dominio.empty() || dominio.length() > 255) {
        throw invalid_argument("Dominio do email deve ter entre 1 e 255 caracteres.");
    }
    if (dominio.front() == '.' || dominio.back() == '.') {
        throw invalid_argument("Dominio nao pode iniciar ou terminar com ponto.");
    }

    stringstream ss(dominio);
    string parte;
    while (getline(ss, parte, '.')) {
        if (parte.empty()) {
            throw invalid_argument("Dominio nao pode conter pontos consecutivos.");
        }
        if (parte.front() == '-' || parte.back() == '-') {
            throw invalid_argument("Parte do dominio nao pode iniciar ou terminar com hifen.");
        }
        for (char c : parte) {
            bool ehLetraMin = (c >= 'a' && c <= 'z');
            bool ehDigito = (c >= '0' && c <= '9');
            bool ehHifen = (c == '-');
            if (!ehLetraMin && !ehDigito && !ehHifen) {
                throw invalid_argument("Caractere invalido no dominio do email.");
            }
        }
    }
}

// ============================================================================
// ESTADO
// ============================================================================
void Estado::validar(string valor) {
    if (valor != "A FAZER" && valor != "FAZENDO" && valor != "FEITO") {
        throw invalid_argument("Estado invalido. Valores aceitos: A FAZER, FAZENDO, FEITO.");
    }
}

// ============================================================================
// IDENTIFICADOR
// ============================================================================
void Identificador::validar(string valor) {
    if (valor.length() != 6) {
        throw invalid_argument("Identificador deve ter exatamente 6 caracteres.");
    }
    for (int i = 0; i < 3; ++i) {
        if (!isalpha(static_cast<unsigned char>(valor[i]))) {
            throw invalid_argument("Os 3 primeiros caracteres do identificador devem ser letras.");
        }
    }
    for (int i = 3; i < 6; ++i) {
        if (!isdigit(static_cast<unsigned char>(valor[i]))) {
            throw invalid_argument("Os 3 ultimos caracteres do identificador devem ser digitos.");
        }
    }
}

// ============================================================================
// LIMITE
// ============================================================================
void Limite::validar(int valor) {
    if (valor < 1 || valor > 25) {
        throw invalid_argument("Limite deve ser um valor entre 1 e 25.");
    }
}

void Limite::setValor(int valor) {
    validar(valor);
    this->valor = valor;
}

// ============================================================================
// NOME
// ============================================================================
void Nome::validar(string valor) {
    if (valor.empty() || valor.length() > 15) {
        throw invalid_argument("Nome deve ter entre 1 e 15 caracteres.");
    }
    if (valor.front() == ' ' || valor.back() == ' ') {
        throw invalid_argument("Nome nao pode iniciar ou terminar com espaco.");
    }
    for (size_t i = 0; i < valor.length(); ++i) {
        char c = valor[i];
        bool ehLetra = ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
        if (!ehLetra && c != ' ') {
            throw invalid_argument("Nome deve conter apenas letras e espacos.");
        }
        if (c == ' ') {
            char prox = valor[i + 1];
            bool proxEhLetra = ((prox >= 'A' && prox <= 'Z') || (prox >= 'a' && prox <= 'z'));
            if (!proxEhLetra) {
                throw invalid_argument("Espaco no nome deve ser seguido por letra.");
            }
        }
    }
}

// ============================================================================
// PAPEL
// ============================================================================
void Papel::validar(string valor) {
    if (valor != "GESTOR" && valor != "DESENVOLVEDOR") {
        throw invalid_argument("Papel invalido. Valores aceitos: GESTOR, DESENVOLVEDOR.");
    }
}

// ============================================================================
// PRIORIDADE
// ============================================================================
void Prioridade::validar(string valor) {
    if (valor != "ALTA" && valor != "MEDIA" && valor != "BAIXA") {
        throw invalid_argument("Prioridade invalida. Valores aceitos: ALTA, MEDIA, BAIXA.");
    }
}

// ============================================================================
// SENHA
// ============================================================================
void Senha::validar(string valor) {
    if (valor.length() != 5) {
        throw invalid_argument("Senha deve ter exatamente 5 caracteres.");
    }
    bool temLetra = false;
    bool temDigito = false;
    for (char c : valor) {
        bool ehLetra = ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
        bool ehDigito = (c >= '0' && c <= '9');
        if (!ehLetra && !ehDigito) {
            throw invalid_argument("Senha deve conter apenas letras e digitos.");
        }
        if (ehLetra) temLetra = true;
        if (ehDigito) temDigito = true;
    }
    if (!temLetra || !temDigito) {
        throw invalid_argument("Senha deve conter pelo menos uma letra e um digito.");
    }
}

// ============================================================================
// TAMANHO
// ============================================================================
void Tamanho::validar(string valor) {
    // Requisito nao funcional 12 dispensou acentuacao, mas aceitamos ambos por seguranca
    if (valor != "GRANDE" && valor != "MEDIO" && valor != "MÉDIO" && valor != "PEQUENO") {
        throw invalid_argument("Tamanho invalido. Valores aceitos: GRANDE, MEDIO, PEQUENO.");
    }
}

// ============================================================================
// TEXTO
// ============================================================================
void Texto::validar(string valor) {
    if (valor.empty() || valor.length() > 30) {
        throw invalid_argument("Texto deve ter entre 1 e 30 caracteres.");
    }
    if (!(valor.front() >= 'A' && valor.front() <= 'Z')) {
        throw invalid_argument("Texto deve iniciar com letra maiuscula.");
    }
    if (valor.back() != '.') {
        throw invalid_argument("Texto deve terminar com ponto final ('.').");
    }

    string pontuacoes = ",;.:?!";
    for (size_t i = 0; i < valor.length(); ++i) {
        char c = valor[i];
        bool ehLetra = ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
        bool ehDigito = (c >= '0' && c <= '9');
        bool ehEspaco = (c == ' ');
        bool ehPontuacao = (pontuacoes.find(c) != string::npos);

        if (!ehLetra && !ehDigito && !ehEspaco && !ehPontuacao) {
            throw invalid_argument("Caractere invalido no texto.");
        }
        if (ehPontuacao && i + 1 < valor.length()) {
            if (pontuacoes.find(valor[i + 1]) != string::npos) {
                throw invalid_argument("Sinal de pontuacao nao pode ser seguido por outro sinal de pontuacao.");
            }
        }
    }
}

// ============================================================================
// TIMESTAMP
// ============================================================================
bool Timestamp::ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

void Timestamp::validar(string valor) {
    // Formato esperado: DIA-MES-ANO-HORARIO (ex: 05-JAN-2024-09:30 ou 5-JAN-2024-09:30)
    vector<string> partes;
    stringstream ss(valor);
    string item;
    while (getline(ss, item, '-')) {
        partes.push_back(item);
    }

    if (partes.size() != 4) {
        throw invalid_argument("Timestamp deve seguir o formato DIA-MES-ANO-HORARIO.");
    }

    string diaStr = partes[0];
    string mesStr = partes[1];
    string anoStr = partes[2];
    string horaStr = partes[3];

    // Validar DIA (1 ou 2 digitos numericos)
    if (diaStr.empty() || diaStr.length() > 2) {
        throw invalid_argument("Dia invalido no timestamp.");
    }
    for (char c : diaStr) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            throw invalid_argument("Dia deve ser numerico.");
        }
    }
    int dia = stoi(diaStr);

    // Validar MES
    vector<string> mesesValidos = {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
                                   "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"};
    auto it = find(mesesValidos.begin(), mesesValidos.end(), mesStr);
    if (it == mesesValidos.end()) {
        throw invalid_argument("Mes invalido. Use siglas de 3 letras maiusculas (ex: JAN, FEV).");
    }
    int mes = distance(mesesValidos.begin(), it) + 1;

    // Validar ANO (4 digitos entre 2000 e 2099)
    if (anoStr.length() != 4) {
        throw invalid_argument("Ano deve conter 4 digitos.");
    }
    for (char c : anoStr) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            throw invalid_argument("Ano deve ser numerico.");
        }
    }
    int ano = stoi(anoStr);
    if (ano < 2000 || ano > 2099) {
        throw invalid_argument("Ano deve estar entre 2000 e 2099.");
    }

    // Validar quantidade de dias no mes considerando ano bissexto
    int diasNoMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (ehBissexto(ano)) {
        diasNoMes[2] = 29;
    }
    if (dia < 1 || dia > diasNoMes[mes]) {
        throw invalid_argument("Dia invalido para o mes/ano informado.");
    }

    // Validar HORARIO (HH:MM de 00:00 a 23:59)
    if (horaStr.length() != 5 || horaStr[2] != ':') {
        throw invalid_argument("Horario deve seguir o formato HH:MM.");
    }
    if (!isdigit(static_cast<unsigned char>(horaStr[0])) ||
        !isdigit(static_cast<unsigned char>(horaStr[1])) ||
        !isdigit(static_cast<unsigned char>(horaStr[3])) ||
        !isdigit(static_cast<unsigned char>(horaStr[4]))) {
        throw invalid_argument("Horario deve conter apenas digitos e ':'.");
    }
    int hora = stoi(horaStr.substr(0, 2));
    int minuto = stoi(horaStr.substr(3, 2));
    if (hora < 0 || hora > 23 || minuto < 0 || minuto > 59) {
        throw invalid_argument("Horario fora do intervalo valido (00:00 a 23:59).");
    }
}
