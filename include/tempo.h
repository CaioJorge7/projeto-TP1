#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include <sstream>
#include <string>
#include <vector>
#include "dominios.h"

// ESCOLHA(Membro3): o dominio Timestamp guarda o valor como texto
//   ("05-JAN-2024-09:30"). Para calcular tempo de ciclo e lead time, esta
//   funcao converte o texto em minutos desde 01-JAN-2000-00:00 (o ano minimo
//   do dominio). Fica aqui, e nao em dominios.h, para nao mexer no trabalho do
//   Membro 1.

/**
 * @brief Diz se um ano e bissexto.
 * @param ano Ano (por exemplo, 2024).
 * @return true se o ano e bissexto.
 */
inline bool anoEhBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

/**
 * @brief Converte um Timestamp em minutos desde 01-JAN-2000-00:00.
 * @param timestamp Timestamp ja validado pelo dominio.
 * @param minutos Recebe o resultado.
 * @return false se o timestamp esta vazio (ainda nao foi definido).
 */
inline bool timestampParaMinutos(const Timestamp& timestamp, long* minutos) {
    string texto = timestamp.getValor();
    if (texto.empty()) return false;

    // Separa DIA-MES-ANO-HORARIO
    vector<string> partes;
    stringstream ss(texto);
    string item;
    while (getline(ss, item, '-')) {
        partes.push_back(item);
    }
    if (partes.size() != 4) return false;

    const char* siglas[] = {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
                            "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"};
    int mes = 0;
    for (int i = 0; i < 12; ++i) {
        if (partes[1] == siglas[i]) mes = i + 1;
    }
    if (mes == 0) return false;

    int dia = stoi(partes[0]);
    int ano = stoi(partes[2]);
    int hora = stoi(partes[3].substr(0, 2));
    int minuto = stoi(partes[3].substr(3, 2));

    int diasNoMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    long dias = 0;
    for (int a = 2000; a < ano; ++a) {
        dias += anoEhBissexto(a) ? 366 : 365;
    }
    for (int m = 1; m < mes; ++m) {
        dias += diasNoMes[m - 1];
        if (m == 2 && anoEhBissexto(ano)) dias += 1;
    }
    dias += dia - 1;

    *minutos = dias * 24 * 60 + hora * 60 + minuto;
    return true;
}

#endif // TEMPO_H_INCLUDED
