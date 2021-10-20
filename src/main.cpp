#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "../lib/mysql/mysql.h"

using namespace std;

struct connection_details {
    const char *server, *user, *password, *database;
};

MYSQL *mysql_connection_setup(struct connection_details mysql_details);
MYSQL_RES *mysql_run(MYSQL *connection, const char *sql_query);
void get_localIP(char *output);
void get_publicIP(char *output);
void host_up(int port);
void host_down(int port);
int socket_check(const char *socket);

char temp[250];
char hostname[20], user[20], port[6], localIP[20], publicIP[20], ID[3];
char command[150];

int main(int argc, char **argv) {
    ifstream file;
    file.open("/usr/lib/libmariadb.so");
    if (file) {
    } else {
        cout << "library files dont exist, run install.sh" << endl;
        exit(0);
    }
    system("cat ./.anyshell/asci.txt");
    printf("\n");

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    struct connection_details anyshell_server;
    anyshell_server.server = "noftp.ddns.net";
    anyshell_server.user = "senaex";
    anyshell_server.password = "Quande-0918";
    anyshell_server.database = "senaex";

    // conn = mysql_connection_setup(anyshell_server);
    // res = mysql_run(conn,
    //     "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, `publicIP`,
    //     `localIP`, `online`, `last-online`, `requested`) " "VALUES ('0',
    //     'Arch-PC', 'senaex', '22', '87.98.45.78', '192.168.178.21', '1',
    //     '2038-01-19 03:14:07', '0');");
    // res = mysql_run(conn, "DELETE FROM server WHERE ID='12'");
    // res = mysql_run(conn, "SELECT * FROM hosts;");
    // res = mysql_run(conn, "SHOW TABLES");
    // res = mysql_run(conn, "SELECT * FROM server WHERE User = 'lennart';");
    // res = mysql_run(conn, "SELECT * FROM server WHERE User LIKE 'len%' AND
    // Domain LIKE 'no%';"); res = mysql_run(conn, "SELECT * FROM server WHERE
    // (User LIKE 'sen%') OR (User LIKE 'len%');"); res = mysql_run(conn,
    // "SELECT * FROM server WHERE User = 'senaex';"); res = mysql_run(conn,
    //     "UPDATE hosts "
    //     "SET localIP='192.197.178.156'"
    //     "WHERE ID=1"
    // );
    // ID, Name, User, Port, publicIP, localIP, requested, last-online, online
    // while ((row = mysql_fetch_row(res)) != NULL){
    //     // the below row[] parametes may change depending on the size of the
    //     table and your objective std::cout << row[0] << " | " << row[1] << "
    //     | " << row[2] << " | " << row[3] << " | " << row[4] << std::endl;
    // }

    // mysql_free_result(res);
    // mysql_close(conn);

    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            // cout << "Listing all hosts...\n" << endl;
            conn = mysql_connection_setup(anyshell_server);
            res = mysql_run(conn, "SELECT * FROM hosts;");
            printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n", "ID",
                   "Hostname", "User", "Port", "public-IP", "local-IP",
                   "online");
            while ((row = mysql_fetch_row(res)) != NULL) {
                printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n",
                       row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
            }

            mysql_free_result(res);
            sprintf(temp,
                    "UPDATE hosts "
                    "SET "
                    "requested='0' "
                    "WHERE Name='Arch-PC';");
                    // "AND Port='22';");
            res = mysql_run(conn, temp);
            mysql_close(conn);
        } else if (strcmp(argv[1], "host") == 0) {
            if (strcmp(argv[2], "setup") == 0) {
                char hostname[20], user[20], port[6], localIP[20], publicIP[20],
                    ID[3];
                cout << "Which Port do you want to share? ";
                cin >> port;
                gethostname(hostname, 20);
                getlogin_r(user, 20);
                get_localIP(localIP);
                get_publicIP(publicIP);

                conn = mysql_connection_setup(anyshell_server);
                res = mysql_run(conn, "SELECT * FROM hosts;");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    if (strcmp(row[1], hostname) == 0) {
                        if (strcmp(row[2], user) == 0) {
                            if (strcmp(row[3], port) == 0) {
                                cout << "\nError: host already exists" << endl;
                                mysql_free_result(res);
                                mysql_close(conn);
                                exit(1);
                            }
                        }
                    }
                }
                int i = 1;
                while (1) {
                    sprintf(temp, "SELECT * FROM hosts WHERE ID='%i';", i);
                    res = mysql_run(conn, temp);
                    if ((row = mysql_fetch_row(res)) == NULL) {
                        sprintf(ID, "%i", i);
                        break;
                    }
                    i++;
                    mysql_free_result(res);
                }

                sprintf(temp,
                        "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, "
                        "`publicIP`, `localIP`, `online`, `last-online`, "
                        "`requested`) "
                        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '1', "
                        "CURRENT_TIMESTAMP, '0');",
                        ID, hostname, user, port, publicIP, localIP);

                mysql_free_result(res);
                res = mysql_run(conn, temp);

                sprintf(temp,
                        "SELECT * FROM hosts WHERE User='%s' AND Name='%s' AND "
                        "Port='%s';",
                        user, hostname, port);
                res = mysql_run(conn, temp);

                printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n",
                       "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "online");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n",
                           row[0], row[1], row[2], row[3], row[4], row[5],
                           row[6]);
                }

                mysql_free_result(res);
                mysql_close(conn);
            } else if (strcmp(argv[2], "deamon") == 0) {
                conn = mysql_connection_setup(anyshell_server);
                gethostname(hostname, 20);
                while (1) {
                    get_localIP(localIP);
                    get_publicIP(publicIP);
                    sprintf(temp,
                            "UPDATE hosts "
                            "SET "
                            "`last-online`=CURRENT_TIMESTAMP, "
                            "`localIP`='%s', "
                            "`publicIP`='%s' "
                            "WHERE Name='%s';",
                            localIP, publicIP, hostname);
                    res = mysql_run(conn, temp);
                    mysql_free_result(res);

                    sprintf(temp, "SELECT * FROM hosts WHERE Name='%s';", hostname);
                    res = mysql_run(conn, temp);
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        if (strcmp(row[8], "1") == 0) {
                            cout << "requested Port " << row[3] << endl;
                            int port = atoi(row[3]);
                            char socket[100];
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            int i = socket_check(socket);
                            cout << i << endl;
                            if (i == 0) {
                                // host_up(port);
                                sprintf(command, "ssh -f -N -T -M -S %s -R 42000:localhost:22 senaex@192.168.178.21 -p 41999 -i ~/.ssh/anyshell-key ", socket);
                                system(command);
                            }
                        }
                        else if (strcmp(row[8], "0") == 0) {
                            int port = atoi(row[3]);
                            char socket[100];
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            sprintf(command, "ssh -S %s -O exit 192.168.178.21", socket);
                            int i = socket_check(socket);
                            cout << i << endl;
                            if (i == 0) {
                                // host_up(port);
                                system(command);
                                i = 1;
                            }
                        }
                    }
                    sleep(1);
                }
            }
        }
    }
    return 0;
}

// system("ssh senaex@192.168.178.21 -p 41999");

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
void host_up(int port) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    struct connection_details anyshell_server;
    anyshell_server.server = "noftp.ddns.net";
    anyshell_server.user = "senaex";
    anyshell_server.password = "Quande-0918";
    anyshell_server.database = "senaex";

    cout << "Hosting on this device, Port " << port << endl;
    conn = mysql_connection_setup(anyshell_server);

    gethostname(hostname, 20);
    getlogin_r(user, 20);

    int i = 1;
    while (1) {
        sprintf(temp, "SELECT * FROM connections WHERE ID='%i';", i);
        res = mysql_run(conn, temp);
        if ((row = mysql_fetch_row(res)) == NULL) {
            sprintf(ID, "%i", i);
            mysql_free_result(res);
            break;
        }
        i++;
        mysql_free_result(res);
    } 
    int Server_port = 41999 + atoi(ID);
    cout << ID << endl;
    cout << port << endl;
    cout << Server_port << endl;

    sprintf(temp,
            "INSERT INTO connections (`ID`, `Name`, `User`, `Host-Port`, `Server-Port`) "
            "VALUES ('%s', '%s', '%s', '%i', '%i');",
            ID, hostname, user, port, Server_port);

    res = mysql_run(conn, temp);

    mysql_free_result(res);
    mysql_close(conn);
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

    sprintf(temp, "DELETE FROM connections WHERE Name='%s' AND Host-Port='%s' AND User='%s';", hostname, port, user);
    res = mysql_run(conn, temp);
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