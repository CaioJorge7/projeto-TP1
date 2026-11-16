# Módulo de Domínios - TP1 (Técnicas de Programação 1)

Implementação completa das classes de domínio, validações, tratamento de exceções (`std::invalid_argument`), documentação no padrão **Doxygen** e testes unitários.

## Estrutura de Arquivos

```text
├── include/
│   └── dominios.h          # Declaração das 11 classes de domínio documentadas (Doxygen)
├── src/
│   └── dominios.cpp        # Implementação das regras de validação e métodos set/get
├── tests/
│   ├── testes_dominios.h   # Declaração das classes de teste unitário
│   └── testes_dominios.cpp # Implementação dos cenários de sucesso e falha
└── main_teste_dominios.cpp # Executável para rodar todos os testes dos domínios
```

## Domínios Implementados
1. `Email`
2. `Estado` (`A FAZER`, `FAZENDO`, `FEITO`)
3. `Identificador` (3 letras + 3 dígitos)
4. `Limite` (1 a 25)
5. `Nome` (até 15 caracteres)
6. `Papel` (`GESTOR`, `DESENVOLVEDOR`)
7. `Prioridade` (`ALTA`, `MEDIA`, `BAIXA`)
8. `Senha` (5 caracteres alfanuméricos com ao menos 1 letra e 1 dígito)
9. `Tamanho` (`GRANDE`, `MEDIO`, `PEQUENO`)
10. `Texto` (até 30 caracteres, inicia com maiúscula e termina com `.`)
11. `Timestamp` (`DIA-MES-ANO-HORARIO` com validação de anos bissextos)

## Como compilar e testar (via terminal ou Code::Blocks)

```bash
g++ -std=c++11 -Iinclude src/dominios.cpp tests/testes_dominios.cpp main_teste_dominios.cpp -o teste_dominios
./teste_dominios
```
