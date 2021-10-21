#include "main.h"

int main(int argc, char **argv) {
    check_install();

    ifstream file("/home/ALEX/anyshell/config.txt");
    string str;
    getline(file, str);
    strcpy(server_user, str.c_str());
    getline(file, str);
    strcpy(server_domain, str.c_str());
    getline(file, str);
    strcpy(server_IP, str.c_str());
    getline(file, str);
    strcpy(server_ssh_port, str.c_str());

    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;

            conn = mysql_connection_setup(anyshell_server);
            print_hosts(conn);

            mysql_close(conn);
        } else if (strcmp(argv[1], "connect") == 0) {
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;

            conn = mysql_connection_setup(anyshell_server);
            print_hosts(conn);

            cout << "\nWhich Host do you want to connect to? ";
            cin >> input;
            request(conn, input);

            cout << "Press any key...";
            int o;
            cin >> o;
            unrequest(conn, input);

            mysql_close(conn);

        } else if (strcmp(argv[1], "host") == 0) {
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;

            if (strcmp(argv[2], "setup") == 0) {
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
                                exit(1);
                            }
                        }
                    }
                }
                get_ID(conn, "hosts", ID);
                sprintf(sql_query,
                        "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, "
                        "`publicIP`, `localIP`, `online`, `last-online`, "
                        "`requested`) "
                        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '1', "
                        "CURRENT_TIMESTAMP, '0');",
                        ID, hostname, user, port, publicIP, localIP);

                mysql_free_result(res);

                res = mysql_run(conn, sql_query);

                sprintf(sql_query,
                        "SELECT * FROM hosts WHERE User='%s' AND Name='%s' AND "
                        "Port='%s';",
                        user, hostname, port);
                res = mysql_run(conn, sql_query);
                printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n",
                       "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "online");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n",
                           row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
                }
                mysql_free_result(res);
                mysql_close(conn);

            } else if (strcmp(argv[2], "deamon") == 0) {
                conn = mysql_connection_setup(anyshell_server);
                gethostname(hostname, 20);
                while (1) {
                    sql_update(conn);

                    sprintf(sql_query, "SELECT * FROM hosts WHERE Name='%s';", hostname);
                    res = mysql_run(conn, sql_query);
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        int check = socket_check(socket);
                        if (strcmp(row[8], "1") == 0) {
                            //Host up
                            cout << "requested Port " << row[3] << endl;
                            int port = atoi(row[3]);
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            if (check == 1) {
                                host_up(port, server_user, server_domain, server_ssh_port);
                            }
                        } else if (strcmp(row[8], "0") == 0) {
                            //Host down
                            cout << "not requested" << endl;
                            int port = atoi(row[3]);
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            if (check == 0) {
                                // host_up(port);
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
