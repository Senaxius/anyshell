#include "main.h"

int main(int argc, char **argv) {
    ifstream file("/opt/anyshell/etc/config.txt");
    string str;
    getline(file, str);
    strcpy(server_user, str.c_str());
    getline(file, str);
    strcpy(server_domain, str.c_str());
    getline(file, str);
    strcpy(server_IP, str.c_str());
    getline(file, str);
    strcpy(server_ssh_port, str.c_str());
    getline(file, str);
    strcpy(server_database, str.c_str());

    ssh_enabled = check_ssh_setup();

    anyshell_server.database = server_database;

    if (argc < 2){
        system("cat /opt/anyshell/etc/asci.txt");
        cout << "\nWelcome to anyshell :) \n" << endl;
        cout << "Commands:" << endl;
        cout << "   list" << endl;
        cout << "   connect" << endl;
        cout << "   host" << endl;
        cout << "       setup" << endl;
        cout << "       remove" << endl;
        cout << "       daemon" << endl;
        cout << "   server" << endl;
    }
    else if (argc > 1) {
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
            unrequest(conn, input);
            cout << "\nWhich Host do you want to connect to? ";
            cin >> input;

            request(conn, input);

            sprintf(sql_query, "SELECT * FROM hosts WHERE `ID`=%i", input);
            res = mysql_run(conn, sql_query);
            while ((row = mysql_fetch_row(res)) != NULL) {
                strcpy(hostname, row[1]);
                strcpy(user, row[2]);
                strcpy(port, row[3]);
                strcpy(localIP, row[5]);
                strcpy(publicIP, row[4]);
            }
            mysql_free_result(res);

            cout << "scanning if host is up: " << flush;
            int b = 0;
            time_t start = time(nullptr);
            int i = 0;
            while (true) {
                if (i == 50) {
                    cout << "." << flush;
                    i = 0;
                }
                i++;
                time_t now = time(nullptr);
                sprintf(sql_query, "SELECT * FROM connections WHERE Name='%s' AND `Host-Port`='%s';",
                        hostname, port);
                res = mysql_run(conn, sql_query);
                while ((row = mysql_fetch_row(res)) != NULL) {
                    strcpy(server_port, row[3]);
                    cout << "found!\n" << endl;
                    b = 1;
                    break;
                }
                mysql_free_result(res);
                if (b == 1) {
                    break;
                }
                if (now - start > 10) {
                    cout << "\nCould not connect to Host!" << endl;
                    unrequest(conn, input);
                    mysql_free_result(res);
                    exit(1);
                }
            }

            char own_IP[20];
            get_publicIP(own_IP);
            int ssh_connect = 1;
            if (argc > 2) {
                if (strcmp(argv[2], "-n") == 0) {
                    ssh_connect = 0;
                }
            }
            
            if (ssh_connect == 1) {
                if (strcmp(own_IP, publicIP) == 0) {
                    cout << "requested host is on same network, connecting localy..." << endl;
                    connect(user, localIP, port);
                } else {
                    cout << "connecting to host via sever..." << endl;
                    system("lsof -t -i:41000 >/dev/null && lsof -ti:41000 | xargs kill -9");
                    sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket %s@%s -p %s -i ~/.ssh/anyshell-key -L 41000:localhost:%s", server_user, server_domain, server_ssh_port, server_port);
                    system(command);

                    strcpy(port, "41000");
                    strcpy(hostname, "localhost");
                    connect(user, hostname, port);

                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket");
                    system(command);
                }
            }
            else{
                if (strcmp(own_IP, publicIP) == 0) {
                    cout << "requested host is on same network, connecting localy..." << endl;
                    sprintf(command, "ssh %s@%s -p %s", user, localIP, port);
                    cout << "\nTo connect, run: \n" << "----------------------------------------------------\n" << command << "\n----------------------------------------------------" << endl;
                    cout << "\nPress any key to stop..." << endl;
                    cin.ignore();
                    cin.ignore();
                } else {
                    cout << "connecting to host via sever..." << endl;
                    system("lsof -t -i:41000 >/dev/null && lsof -ti:41000 | xargs kill -9");
                    sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket %s@%s -p %s -i ~/.ssh/anyshell-key -L 41000:localhost:%s", server_user, server_domain, server_ssh_port, server_port);
                    system(command);

                    sprintf(command, "ssh %s@localhost -p 41000", user);
                    cout << "\nTo connect, run: \n" << "----------------------------------------------------\n" << command << "\n----------------------------------------------------" << endl;
                    cout << "\nPress any key to stop..." << endl;
                    cin.ignore();
                    cin.ignore();

                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket");
                    system(command);
                }
            }

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
                get_user(user);
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
                printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %s \n",
                       "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "online");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %s \n",
                           row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
                }
                mysql_free_result(res);
                mysql_close(conn);

            } else if (strcmp(argv[2], "daemon") == 0) {
                conn = mysql_connection_setup(anyshell_server);
                gethostname(hostname, 20);
                while (1) {
                    sql_update(conn);
                    sprintf(sql_query, "SELECT * FROM hosts WHERE Name='%s';", hostname);
                    res = mysql_run(conn, sql_query);
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        if (strcmp(row[8], "1") == 0) {
                            //Host up
                            int port = atoi(row[3]);
                            cout << "Host up, port: " << port << endl;
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            int check = socket_check(socket);
                            if (check == 0) {
                                if (ssh_enabled == 0) {
                                    system("systemctl start sshd.service");
                                }
                                host_up(anyshell_server.database, port, server_user, server_domain, server_ssh_port);
                            }
                        } else if (strcmp(row[8], "0") == 0) {
                            //Host down
                            int port = atoi(row[3]);
                            cout << "Host down, port: " << port << endl;
                            sprintf(socket, "/opt/anyshell/etc/host_socket_%i", port);
                            int check = socket_check(socket);
                            if (check == 1) {
                                host_down(anyshell_server.database, port, server_domain);
                                if (ssh_enabled == 0) {
                                    system("systemctl stop sshd.service");
                                }
                            }
                        }
                    }
                    sleep(1);
                }
            } else if (strcmp(argv[2], "remove") == 0) {
                conn = mysql_connection_setup(anyshell_server);

                cout << "Which Port do you want to remove? ";
                cin >> port;
                cout << "Removing this device from hosts, port: " << port << endl;

                gethostname(hostname, 20);
                getlogin_r(user, 20);

                sprintf(sql_query,
                        "DELETE FROM hosts "
                        "WHERE `Name`='%s' "
                        "AND `Port`='%s' "
                        "AND `User`='%s';",
                        hostname, port, user);

                res = mysql_run(conn, sql_query);

                mysql_free_result(res);
                mysql_close(conn);

                cout << "done!" << endl;
            }

        } else if (strcmp(argv[1], "server") == 0) {
            cout << "starting anyshell server..." << endl;
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;

            while (1) {
                cout << "updating list..." << endl;
                anyshell_server.database = "senaex";
                conn = mysql_connection_setup(anyshell_server);
                sprintf(sql_query, "UPDATE hosts SET `online`='0' WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                res = mysql_run(conn, sql_query);
                mysql_close(conn);
                anyshell_server.database = "lennart";
                conn = mysql_connection_setup(anyshell_server);
                sprintf(sql_query, "UPDATE hosts SET `online`='0' WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                res = mysql_run(conn, sql_query);
                mysql_close(conn);
                sleep(1);
            }
        } else if (strcmp(argv[1], "reload") == 0) {
            system("make -C /opt/anyshell clean && make -C /opt/anyshell -j8");
            system("sudo systemctl restart anyshell-daemon.service");
            system("sudo systemctl restart anyshell-server.service");
        }
    }
    return 0;
}
