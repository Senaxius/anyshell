#include "main.h"

void check_install() {
    file.open("/usr/lib/libmariadb.so");
    if (file) {
    } else {
        cout << "library files dont exist, run install.sh" << endl;
        exit(0);
    }
}

void print_hosts(MYSQL *conn) {
    // MYSQL_RES *res;
    // MYSQL_ROW row;
    // res = mysql_run(conn, "SELECT * FROM hosts;");
    // printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %-19s | %s \n",
    //        "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "last-online", "online");
    // while ((row = mysql_fetch_row(res)) != NULL) {
    //     string time = row[7];
    //     time.erase(time.size() - 7);
    //     printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %-19s | %s \n",
    //            row[0], row[1], row[2], row[3], row[4], row[5], time.c_str(), row[6]);
    // }
    MYSQL_RES *res;
    MYSQL_ROW row;
    res = mysql_run(conn, "SELECT * FROM hosts;");
    printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %-5s | %s \n",
           "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "req", "online");
    while ((row = mysql_fetch_row(res)) != NULL) {
        string time = row[7];
        time.erase(time.size() - 7);
        printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %-5s | %s \n",
               row[0], row[1], row[2], row[3], row[4], row[5], row[8], row[6]);
    }
    mysql_free_result(res);
}

void request(MYSQL *conn, int ID){
    MYSQL_RES *res;
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "requested='1' "
            "WHERE ID='%i'",
            ID);
    res = mysql_run(conn, sql_query);
}
void unrequest(MYSQL *conn, int ID){
    MYSQL_RES *res;
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "requested='0' "
            "WHERE ID='%i'",
            ID);
    res = mysql_run(conn, sql_query);
}

void get_ID(MYSQL *conn, const char* table, char * ID){
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

void sql_update(MYSQL *conn){
    MYSQL_RES *res;
    MYSQL_ROW row;

    gethostname(hostname, 20);
    get_localIP(localIP);
    get_publicIP(publicIP);
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "`last-online`=CURRENT_TIMESTAMP, "
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

void host_up(int port, char *ssh_user, char *ssh_host, char *ssh_port) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    cout << "Hosting on this device..." << endl;
    conn = mysql_connection_setup(anyshell_server);

    gethostname(hostname, 20);
    getlogin_r(user, 20);
    get_ID(conn, "connections", ID);

    int Server_port = 41999 + atoi(ID);

    sprintf(sql_query,
            "INSERT INTO connections (`ID`, `Name`, `User`, `Host-Port`, `Server-Port`) "
            "VALUES ('%s', '%s', '%s', '%i', '%i');",
            ID, hostname, user, port, Server_port);
    res = mysql_run(conn, sql_query);

    sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
    sprintf(command, "ssh -f -N -T -M -S %s -R %i:localhost:%i %s@%s -p %s -i ~/.ssh/anyshell-key ", socket, Server_port, port, ssh_user, ssh_host, ssh_port);
    system(command);

    mysql_free_result(res);
    mysql_close(conn);
    cout << "done" << endl;
}
void host_down(int port) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    struct connection_details anyshell_server;
    anyshell_server.server = "noftp.ddns.net";
    anyshell_server.user = "senaex";
    anyshell_server.password = "Quande-0918";
    anyshell_server.database = "senaex";

    cout << "Stop hosting on this device, Port " << port << endl;
    conn = mysql_connection_setup(anyshell_server);

    gethostname(hostname, 20);
    getlogin_r(user, 20);

    sprintf(sql_query, "DELETE FROM connections WHERE Name='%s' AND Host-Port='%s' AND User='%s';", hostname, port, user);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);
    mysql_close(conn);
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
                            mysql_details.database, 0, NULL, 0)) {
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
