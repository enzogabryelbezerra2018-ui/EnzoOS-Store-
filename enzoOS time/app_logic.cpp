#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <map>
#include <string>

// Arquivo para salvar país/fuso horário selecionado
const std::string CONFIG_FILE = "user_timezone.cfg";

// Lista de países e offsets (simplificado)
std::map<std::string, int> timezone_offsets = {
    {"UTC", 0}, {"Brasil", -3}, {"EUA_Eastern", -5}, {"Reino Unido", 0},
    {"Japão", 9}, {"Austrália", 10}, {"Índia", 5} // adicione mais países/fusos
};

// Lê o fuso configurado pelo usuário (ou retorna UTC se não houver)
std::string load_user_timezone() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return "UTC";
    std::string tz;
    std::getline(file, tz);
    file.close();
    if (timezone_offsets.find(tz) != timezone_offsets.end()) return tz;
    return "UTC";
}

// Salva o fuso selecionado
void save_user_timezone(const std::string& tz) {
    std::ofstream file(CONFIG_FILE, std::ios::trunc);
    file << tz;
    file.close();
}

// Retorna hora atual formatada como HH:MM:SS ajustada ao fuso
std::string get_time_for_timezone(const std::string& tz) {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm* ptm = std::gmtime(&t); // hora UTC

    int offset = timezone_offsets[tz]; // offset em horas
    ptm->tm_hour += offset;
    std::mktime(ptm); // normaliza a hora

    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", ptm);
    return std::string(buffer);
}

// Função para mostrar tela de setup
std::string show_setup_screen() {
    std::cout << "Selecione seu país/fuso horário:\n";
    int i = 1;
    for (auto& kv : timezone_offsets) {
        std::cout << i << ") " << kv.first << "\n";
        i++;
    }
    int choice = 0;
    std::cin >> choice;
    if (choice < 1 || choice > (int)timezone_offsets.size()) choice = 1;
    auto it = timezone_offsets.begin();
    std::advance(it, choice-1);
    return it->first;
}

// Função para mostrar horário principal
void show_main_screen(const std::string& user_tz) {
    while (true) {
        system("clear"); // limpa terminal para modo escuro (console)
        std::cout << "=== Enzo World Clock (Modo Escuro) ===\n";
        std::cout << "Seu país/fuso: " << user_tz << "\n";
        std::cout << "Hora atual: " << get_time_for_timezone(user_tz) << "\n\n";
        std::cout << "Outros horários:\n";
        for (auto& kv : timezone_offsets) {
            if (kv.first == user_tz) continue;
            std::cout << kv.first << ": " << get_time_for_timezone(kv.first) << "\n";
        }
        std::cout << "\nPressione Ctrl+C para sair ou reinicie para mudar fuso.\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::string user_tz = load_user_timezone();
    if (user_tz == "UTC") {
        user_tz = show_setup_screen();
        save_user_timezone(user_tz);
    }
    show_main_screen(user_tz);
    return 0;
}
