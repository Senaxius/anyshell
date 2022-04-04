#include "main.h"

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

bool isNumber(char number[]) {
    int i = 0;
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++) {
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}

void get_databases(list<string> &databases) {
    ifstream data("/opt/anyshell/etc/databases.txt");
    while (data.is_open()) {
        while (getline(data, str)) {
            databases.push_back(str);
        }
        data.close();
    }
}

void get_server_config(const std::string &config_path, server_details *server) {
    ifstream file(config_path);
    string str;

    getline(file, str);
    server->user = strdup(str.c_str());
    getline(file, str);
    server->domain = strdup(str.c_str());
    getline(file, str);
    server->password = strdup(str.c_str());
    getline(file, str);
    server->database = strdup(str.c_str());
    getline(file, str);
    server->SQL_port = strdup(str.c_str());
    getline(file, str);
    server->SSH_port = strdup(str.c_str());

    file.close();
}

void get_user_config(user_details *anyshell_user) {
    get_user(anyshell_user->user);
    get_hostname(anyshell_user->hostname);
    get_localIP(anyshell_user->localIP);
    get_publicIP(anyshell_user->publicIP);
    anyshell_user->ssh_enabled = check_ssh_setup();
}
void get_user(char *user) {
    string temp;
    temp = exec("whoami");
    temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
    strcpy(user, temp.c_str());
}
void get_hostname(char *hostname) {
    string temp;
    temp = exec("hostname");
    temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
    strcpy(hostname, temp.c_str());
}
void get_localIP(char *output) {
    string temp;
    // temp = exec(R"(ifconfig | sed -En 's/127.0.0.1//;s/.*inet (addr:)?(([0-9]*\.){3}[0-9]*).*/\2/p')");
    temp = exec(R"(ip -o -4  address show  | awk ' NR==2 { gsub(/\/.*/, "", $4); print $4 } ')");
    temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
    strcpy(output, temp.c_str());
}
void get_publicIP(char *output) {
    string temp;
    temp = exec(R"(curl -s ifconfig.me | grep -o -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')");
    temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
    strcpy(output, temp.c_str());
}
int check_ssh_setup() {
    string temp;
    temp = exec("systemctl is-enabled sshd.service > /dev/null 2>&1 && echo 1 || echo 0");
    if (strcmp(temp.c_str(), "1") == 0)
        return 1;
    else
        return 0;
}

void print_hosts(MYSQL *conn, int verbose) {
    if (verbose == 1) {
        res = mysql_run(conn, "SELECT * FROM hosts ORDER BY `ID` ASC;");
        printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %-19s | %s \n",
               "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "last-online", "online");
        while ((row = mysql_fetch_row(res)) != NULL) {
            string time = row[7];
            printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %-19s | %s \n",
                   row[0], row[1], row[2], row[3], row[4], row[5], time.c_str(), row[6]);
        }
        mysql_free_result(res);
    } else {
        res = mysql_run(conn, "SELECT * FROM hosts WHERE ID < 10 ORDER BY `ID` ASC;");
        printf("%-3s | %-14s | %-8s | %-5s | %s \n",
               "ID", "Hostname", "User", "Port", "online");
        while ((row = mysql_fetch_row(res)) != NULL) {
            printf("%-3s | %-14s | %-8s | %-5s | %s \n",
                   row[0], row[1], row[2], row[3], row[6]);
        }
        mysql_free_result(res);
    }
}

void print_help() {
    system("cat /opt/anyshell/etc/asci.txt");
    cout << "\nWelcome to anyshell :)" << endl;
    cout << "\nCurrent Version: V.2.1\n"
         << endl;
    cout << "Commands:" << endl;
    cout << "   list" << endl;
    cout << "       -v" << endl;
    cout << "   connect" << endl;
    cout << "       -v" << endl;
    cout << "       -s" << endl;
    cout << "   host" << endl;
    cout << "       setup" << endl;
    cout << "       remove" << endl;
    cout << "       daemon" << endl;
    cout << "   server" << endl;
    cout << "   upgrade" << endl;
    cout << "   change" << endl;
}

void get_ID(MYSQL *conn, const char *table, char *ID) {
    int i = 1;
    while (1) {
        sprintf(sql_query, "SELECT * FROM %s WHERE ID='%i';", table, i);
        res = mysql_run(conn, sql_query);
        if ((row = mysql_fetch_row(res)) == NULL) {
            sprintf(ID, "%i", i);
            break;
        }
        i++;
        mysql_free_result(res);
    }
}

void remove_from_list(list<int> *list, int a) {
    for (int i = 0; i < list->size(); i++) {
        auto it = list->begin();
        std::advance(it, i);
        if (*it == a) {
            list->erase(it);
            break;
        }
    }
}

int check_connection(list<int> *list, int a) {
    int b = 0;
    for (int i = 0; i < list->size(); i++) {
        auto it = list->begin();
        std::advance(it, i);
        if (*it == a) {
            b = 1;
            break;
        }
    }
    return b;
}

void ctrl_c_handler(int s) {
    cout << "\nexiting anyshell..." << endl;
    // mysql_free_result(res);
    // unrequest(conn, &anyshell_host);
    exit(0);
}

void setting_ctrl_c() {
    // setting up ctrl_c action
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}
