#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PACIENTES 100
#define MAX_SALAS 10
#define MAX_MEDICOS 20
#define MAX_ESPECIALIDADES 10

// Estrutura para representar um paciente
typedef struct {
    int id;
    char nome[50];
    int idade;
    char telefone[15];
    float peso;
    float altura;
    char sintomas[100];
    char medicacoes[100];
    int prioridade;
    int faltou; // 0 para não, 1 para sim
} Paciente;

// Estrutura para representar um médico
typedef struct {
    int id;
    char nome[50];
    char especialidade[50];
    int horas_trabalhadas;
} Medico;

// Estrutura para representar uma sala
typedef struct {
    int id;
    int ocupada; // 0 para não, 1 para sim
} Sala;

// Estrutura para representar uma especialidade
typedef struct {
    char nome[50];
    Medico medicos[MAX_MEDICOS];
    int num_medicos;
} Especialidade;

// Função para ler os dados de um arquivo
void lerDados(const char*filename, Paciente pacientes[], int*num_pacientes, Sala salas[], int*num_salas, Especialidade especialidades[], int*num_especialidades) {
    FILE*file = fopen(filename, "r");
    int i;
    int j;
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", num_pacientes);
    fscanf(file, "%d", num_salas);
    fscanf(file, "%d", num_especialidades);

    for ( i = 0; i < *num_especialidades; i++) {
        fscanf(file, "%s", especialidades[i].nome);
        fscanf(file, "%d", &especialidades[i].num_medicos);
        for ( j = 0; j < especialidades[i].num_medicos; j++) {
            fscanf(file, "%d %s", &especialidades[i].medicos[j].id, especialidades[i].medicos[j].nome);
            strcpy(especialidades[i].medicos[j].especialidade, especialidades[i].nome);
            especialidades[i].medicos[j].horas_trabalhadas = 0;
        }
    }

    for ( i = 0; i < *num_pacientes; i++) {
        fscanf(file, "%d %s %d %s %f %f %s %s %d", &pacientes[i].id, pacientes[i].nome, &pacientes[i].idade, pacientes[i].telefone, &pacientes[i].peso, &pacientes[i].altura, pacientes[i].sintomas, pacientes[i].medicacoes, &pacientes[i].prioridade);
        pacientes[i].faltou = rand() % 20 < 1 ? 1 : 0; // 5% de chance de faltar
    }

    fclose(file);
}

// Função para comparar prioridades de pacientes (para qsort)
int compararPrioridade(const void* a, const void* b) {
    Paciente* pacienteA = (Paciente*)a;
    Paciente* pacienteB = (Paciente*)b;
    return pacienteB->prioridade - pacienteA->prioridade;
}

// Função para comparar horas trabalhadas de médicos
int compararHorasTrabalhadas(const void* a, const void* b) {
    Medico* medicoA = (Medico*)a;
    Medico* medicoB = (Medico*)b;
    return medicoB->horas_trabalhadas - medicoA->horas_trabalhadas;
}

// Função para alocar consultas
void alocarConsultas(Paciente pacientes[], int num_pacientes, Sala salas[], int num_salas, Especialidade especialidades[], int num_especialidades) {
    qsort(pacientes, num_pacientes, sizeof(Paciente), compararPrioridade); // Ordena os pacientes por prioridade

    int semana = 1;
    int hora = 8;
    int dia = 1;
    int sala_atual = 0;
    int i;
    int j;
    int k;

    for ( i = 0; i < num_pacientes; i++) {
        if (pacientes[i].faltou) {
            pacientes[i].prioridade -= 1;
            continue;
        }

        Medico* medico = NULL;

        // Busca o médico adequado para os sintomas do paciente
        for ( j = 0; j < num_especialidades; j++) {
            for ( k = 0; k < especialidades[j].num_medicos; k++) {
                if (strcmp(especialidades[j].medicos[k].especialidade, pacientes[i].sintomas) == 0) {
                    medico = &especialidades[j].medicos[k];
                    break;
                }
            }
            if (medico) break;
        }

        if (!medico) continue;  // Se não encontrou médico adequado, pula para o próximo paciente

        // Aloca a consulta na sala
        if (salas[sala_atual].ocupada == 0) {
            salas[sala_atual].ocupada = 1; // Marca a sala como ocupada
            printf("Semana %d, Dia %d, Sala %d, Hora %02d:00 - Paciente: %s, Médico: %s\n", semana, dia, salas[sala_atual].id, hora, pacientes[i].nome, medico->nome);
            medico->horas_trabalhadas++; // Incrementa as horas trabalhadas do médico
            hora++; // Avança uma hora

            if (hora == 18) {
                hora = 8;
                dia++;
                if (dia == 6) {
                    dia = 1;
                    semana++;
                }
            }

            sala_atual++;
            if (sala_atual == num_salas) {
                sala_atual = 0;
            }
        }
    }
}

// Função para gerar relatório de horas trabalhadas
void gerarRelatorio(Medico medicos[], int num_medicos) {
    qsort(medicos, num_medicos, sizeof(Medico), compararHorasTrabalhadas); // Ordena os médicos por horas trabalhadas
	int i;
    printf("\nRelatorio de Horas Trabalhadas:\n");
    for ( i = 0; i < num_medicos; i++) {
        printf("Medico: %s, Horas Trabalhadas: %d\n", medicos[i].nome, medicos[i].horas_trabalhadas);
    }
}

int main() {
    srand(time(NULL));
    int i;
    int j;

    Paciente pacientes[MAX_PACIENTES];
    Sala salas[MAX_SALAS];
    Especialidade especialidades[MAX_ESPECIALIDADES];

    int num_pacientes, num_salas, num_especialidades;
    lerDados("dados.txt", pacientes, &num_pacientes, salas, &num_salas, especialidades, &num_especialidades);

    alocarConsultas(pacientes, num_pacientes, salas, num_salas, especialidades, num_especialidades);

    Medico medicos[MAX_MEDICOS];
    int num_medicos = 0;

    // Coleta todos os médicos de todas as especialidades
    for ( i = 0; i < num_especialidades; i++) {
        for ( j = 0; j < especialidades[i].num_medicos; j++) {
            medicos[num_medicos++] = especialidades[i].medicos[j];
        }
    }

    gerarRelatorio(medicos, num_medicos);

    return 0;
}

