#include "main.h"

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void connect(char *user, char *host, char *port){
    sprintf(command, "/opt/anyshell/lib/connect.sh %s %s %s", user, host, port);
    system(command);
}

void get_user(char *user){
    string temp;
    temp = exec("whoami");
    temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end());
    temp.erase(remove(temp.begin(), temp.end(), '\n'), temp.end());
    strcpy(user, temp.c_str());
}
int check_ssh(char *user, char *host, char *port){
    FILE *fp;
    char check[5];
    int temp, b;
    for (int i=0; i<10; i++){
        sprintf(command, "ssh -q %s@%s -p %s", user, host, port);
        fp = popen(command, "r");
        while (fgets(check, sizeof(check), fp) != NULL) {}
        for (int i = 0; i < 5; ++i) {
            if (isdigit(check[i])) {
                temp = (int)check[i] - 48;
                if (temp == 1){
                    b = 1;
                    break;
                } else if (temp == 0){
                    b = 0;
                    break;
                }
            }
        }
        cout << i << endl;
        if (b == 1){
            break;
        }
        usleep(500000);
    }
    pclose(fp);
    return temp;
}

int check_ssh_setup(){
    FILE *fp;
    char check[10];
    int temp;
    fp = popen(
        R"(systemctl is-enabled sshd.service >/dev/null 2>&1 && echo 1 || echo 0)",
        "r");
    while (fgets(check, sizeof(check), fp) != NULL) {
    }
    pclose(fp);
    for (int i = 0; i < 10; ++i) {
        if (isdigit(check[i])) {
            temp = (int)check[i] - 48;
        }
    }
    return temp;
}

void print_hosts(MYSQL *conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    res = mysql_run(conn, "SELECT * FROM hosts ORDER BY `ID` ASC;");
    printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %-19s | %s \n",
           "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "last-online", "online");
    while ((row = mysql_fetch_row(res)) != NULL) {
        string time = row[7];
        time.erase(time.size() - 7);
        printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %-19s | %s \n",
               row[0], row[1], row[2], row[3], row[4], row[5], time.c_str(), row[6]);
    }
    mysql_free_result(res);
}

void request(MYSQL *conn, int ID) {
    MYSQL_RES *res;
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "requested='1' "
            "WHERE ID='%i'",
            ID);
    res = mysql_run(conn, sql_query);
}
void unrequest(MYSQL *conn, int ID) {
    MYSQL_RES *res;
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "requested='0' "
            "WHERE ID='%i'",
            ID);
    res = mysql_run(conn, sql_query);
}

void get_ID(MYSQL *conn, const char *table, char *ID) {
    MYSQL_RES *res;
    MYSQL_ROW row;

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

void sql_update(MYSQL *conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    gethostname(hostname, 20);
    get_localIP(localIP);
    get_publicIP(publicIP);
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "`last-online`=CURRENT_TIMESTAMP, "
            "`online`='1', "
            "`localIP`='%s', "
            "`publicIP`='%s' "
            "WHERE Name='%s';",
            localIP, publicIP, hostname);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);
}

void get_localIP(char *output) {
    FILE *fp;
    char IP[20];
    fp = popen(
        R"(ifconfig | sed -En 's/127.0.0.1//;s/.*inet (addr:)?(([0-9]*\.){3}[0-9]*).*/\2/p')",
        "r");
    while (fgets(IP, sizeof(IP), fp) != NULL) {
    }
    pclose(fp);
    for (int i = 0; i < 20; ++i) {
        if (isdigit(IP[i])) {
            output[i] = IP[i];
        } else if (IP[i] == '.') {
            output[i] = IP[i];
        } else {
            output[i] = '\0';
        }
    }
}
void get_publicIP(char *output) {
    FILE *fp;
    char IP[20];
    fp = popen(
        R"(curl -s ifconfig.me | grep -o -Eo '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')",
        "r");
    while (fgets(IP, sizeof(IP), fp) != NULL) {
    }
    pclose(fp);
    for (int i = 0; i < 20; ++i) {
        if (isdigit(IP[i])) {
            output[i] = IP[i];
        } else if (IP[i] == '.') {
            output[i] = IP[i];
        } else {
            output[i] = '\0';
        }
    }
}

void host_up(const char *database, int port, char *ssh_user, char *ssh_host, char *ssh_port) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    anyshell_server.database = database;
    conn = mysql_connection_setup(anyshell_server);
    cout << "Hosting on this device, port: " << port << endl;

    gethostname(hostname, 20);
    getlogin_r(user, 20);
    get_localIP(localIP);
    get_publicIP(publicIP);
    get_ID(conn, "connections", ID);
    sprintf(server_port, "%i", (41999 + atoi(ID)));

    sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
    sprintf(command, "ssh -f -N -T -M -S %s -R %s:localhost:%i %s@%s -p %s -i ~/.ssh/anyshell-key ", socket, server_port, port, ssh_user, ssh_host, ssh_port);
    system(command);

    sprintf(sql_query,
            "INSERT INTO connections (`ID`, `Name`, `Host-Port`, `Server-Port`) "
            "VALUES ('%s', '%s', '%i', '%s');",
            ID, hostname, port, server_port);

    res = mysql_run(conn, sql_query);

    mysql_free_result(res);
    mysql_close(conn);

    cout << "done" << endl;
}
void host_down(const char *database, int port, char *ssh_host) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    anyshell_server.database = database;
    conn = mysql_connection_setup(anyshell_server);

    cout << "Stop hosting on this device, port: " << port << endl;

    gethostname(hostname, 20);
    getlogin_r(user, 20);

    sprintf(sql_query,
            "DELETE FROM connections "
            "WHERE `Name`='%s' "
            "AND `Host-Port`='%i';",
            hostname, port);

    res = mysql_run(conn, sql_query);

    mysql_free_result(res);
    mysql_close(conn);

    sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
    sprintf(command, "ssh -S %s -O exit %s", socket, ssh_host);
    system(command);

    cout << "done" << endl;
}
int socket_check(const char *input) {
    char y[5];
    FILE *fp;
    sprintf(command, "if [ -S %s ]; then echo 1; else echo 0; fi", input);

    fp = popen(command, "r");
    while (fgets(y, sizeof(y), fp) != NULL) {
    }
    pclose(fp);
    if (strstr(y, "1")) {
        return 1;
    } else {
        return 0;
    }
}
MYSQL *mysql_connection_setup(struct connection_details mysql_details) {
    MYSQL *connection = mysql_init(NULL);

    // connect database
    if (!mysql_real_connect(connection, mysql_details.server,
                            mysql_details.user, mysql_details.password,
                            mysql_details.database, 41998, NULL, 0)) {
        std::cout << "Connection Error: " << mysql_error(connection)
                  << std::endl;
        exit(1);
    }
    return connection;
}

MYSQL_RES *mysql_run(MYSQL *connection, const char *sql_query) {
    // send query to db
    if (mysql_query(connection, sql_query)) {
        std::cout << "MySQL Query Error: " << mysql_error(connection)
                  << std::endl;
        exit(1);
    }
    return mysql_use_result(connection);
}
