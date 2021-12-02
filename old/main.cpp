#include "main.h"

int main(int argc, char **argv) {
    // get_config("/opt/anyshell/etc/config.txt", &anyshell_server);
    // // char user[] = "senaex";
    // // anyshell_server.user = user;
    // cout << anyshell_server.user << endl;

    exit(0);
    ssh_enabled = check_ssh_setup();

    list<string> databases;
    ifstream data("/opt/anyshell/etc/databases.txt");
    while (data.is_open()) {
        while (getline(data, str)) {
            databases.push_back(str);
        }
        data.close();
    }

    if (argc < 2) {
        print_help();
    } else if (argc > 1) {
/*****************************list******************************/
        if (strcmp(argv[1], "list") == 0) {
            conn = mysql_connection_setup(anyshell_server);
            print_hosts(conn);
            mysql_close(conn);
/****************************connect****************************/
        } else if (strcmp(argv[1], "connect") == 0) {
            conn = mysql_connection_setup(anyshell_server);
            print_hosts(conn);

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
                i++;
                if (i == 50) {
                    cout << "." << flush;
                    i = 0;
                }
                time_t now = time(nullptr);

                sprintf(sql_query, "SELECT * FROM connections WHERE Name='%s' AND `Host-Port`='%s';", hostname, port);
                res = mysql_run(conn, sql_query);

                while ((row = mysql_fetch_row(res)) != NULL) {
                    strcpy(server_port, row[3]);
                    cout << "found!\n"
                         << endl;
                    b = 1;
                    break;
                }
                mysql_free_result(res);
                if (b == 1) {
                    break;
                }
                if (now - start > 8) {
                    cout << "\nCould not connect to Host!" << endl;
                    unrequest(conn, input);
                    mysql_free_result(res);
                    exit(1);
                }
            }

            char own_IP[20];
            int ssh_connect = 1;
            if (argc > 2) {
                if (strcmp(argv[2], "-n") == 0) {
                    ssh_connect = 0;
                }
                if (strcmp(argv[2], "-s") == 1) {
                    get_publicIP(own_IP);
                }
            } else {
                get_publicIP(own_IP);
            }

            if (ssh_connect == 1) {
                if (strcmp(own_IP, publicIP) == 0) {
                    cout << "requested host is on same network, connecting localy..." << endl;
                    connect(user, localIP, port);
                } else {
                    cout << "connecting to host via sever..." << endl;
                    sprintf(command, "lsof -t -i:%s >/dev/null && lsof -ti:%s | xargs kill -9", server_port, server_port);
                    system(command);
                    sprintf(command, "rm -f /opt/anyshell/etc/guest_socket_%s", server_port);
                    system(command);
                    sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket_%s %s@%s -p %s -i ~/.ssh/anyshell-key -L %s:localhost:%s", socket_number, server_user, server_domain, server_ssh_port, server_port, server_port);
                    system(command);

                    strcpy(hostname, "localhost");
                    connect(user, hostname, server_port);

                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket_%s", server_port, server_port);
                    system(command);
                }
            } else {
                if (strcmp(own_IP, publicIP) == 0) {
                    cout << "requested host is on same network, connecting localy..." << endl;
                    sprintf(command, "ssh %s@%s -p %s", user, localIP, port);
                    cout << "\nTo connect, run: \n"
                         << "----------------------------------------------------\n"
                         << command << "\n----------------------------------------------------" << endl;
                    cout << "\nPress any key to stop..." << endl;
                    cin.ignore();
                    cin.ignore();
                } else {
                    cout << "connecting to host via sever..." << endl;
                    sprintf(command, "lsof -t -i:%s >/dev/null && lsof -ti:%s | xargs kill -9", server_port, server_port);
                    system(command);
                    sprintf(command, "rm -f /opt/anyshell/etc/guest_socket_%s", server_port);
                    system(command);
                    sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket_%s %s@%s -p %s -i ~/.ssh/anyshell-key -L %s:localhost:%s", server_port, server_user, server_domain, server_ssh_port, server_port, server_port);
                    system(command);

                    sprintf(command, "ssh %s@localhost -p %s", user, server_port);
                    cout << "\nTo connect, run: \n"
                         << "----------------------------------------------------\n"
                         << command << "\n----------------------------------------------------" << endl;
                    cout << "\nPress any key to stop..." << endl;
                    cin.ignore();
                    cin.ignore();

                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket_%s", server_port, server_port);
                    system(command);
                }
            }

            // unrequest(conn, input);
            mysql_close(conn);

        } else if (strcmp(argv[1], "host") == 0) {
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;

/***************************host-setup**************************/
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

/**************************host-daemon**************************/
            } else if (strcmp(argv[2], "daemon") == 0) {
                while (1) {
                    for (auto const &database : databases) {
                        anyshell_server.database = database.c_str();
                        conn = mysql_connection_setup(anyshell_server);
                        gethostname(hostname, 20);
                        sql_update(conn);
                        sprintf(sql_query, "SELECT * FROM hosts WHERE Name='%s';", hostname);
                        res = mysql_run(conn, sql_query);
                        while ((row = mysql_fetch_row(res)) != NULL) {
                            if (strcmp(row[8], "0") == 0) {
                                // Host down
                                int port = atoi(row[3]);
                                cout << database.c_str() << ": \t"
                                     << "Host down, port: " << port << "   " << ssh_enabled << endl;
                                sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database.c_str(), port);
                                int check = socket_check(socket);
                                if (check == 1) {
                                    host_down(anyshell_server.database, port, server_domain);
                                    if (ssh_enabled == 0) {
                                        system("systemctl stop sshd.service");
                                    }
                                }
                            } else {
                                // Host up
                                int port = atoi(row[3]);
                                cout << database.c_str() << ": \t"
                                     << "Host up, port: " << port << "   " << ssh_enabled << endl;
                                sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database.c_str(), port);
                                int check = socket_check(socket);
                                if (check == 0) {
                                    if (ssh_enabled == 0) {
                                        system("systemctl start sshd.service");
                                    }
                                    host_up(anyshell_server.database, port, server_user, server_domain, server_ssh_port);
                                }
                            }
                            mysql_close(conn);
                        }
                        sleep(1);
                    }
                }
/**************************host-remove**************************/
            } else if (strcmp(argv[2], "remove") == 0) {
                conn = mysql_connection_setup(anyshell_server);

                cout << "Which Port do you want to remove? ";
                cin >> port;
                cout << "Removing this device from hosts, port: " << port << endl;

                gethostname(hostname, 20);
                get_user(user);

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

/*****************************server****************************/
        } else if (strcmp(argv[1], "server") == 0) {
            cout << "starting anyshell server..." << endl;
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;
            while (1) {
                for (auto const &database : databases) {
                    cout << "updating list..." << endl;
                    anyshell_server.database = database.c_str();
                    conn = mysql_connection_setup(anyshell_server);
                    sprintf(sql_query, "UPDATE hosts SET `online`='0' WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                    res = mysql_run(conn, sql_query);
                    mysql_close(conn);
                }
                sleep(1);
            }
        } else if (strcmp(argv[1], "reload") == 0) {
            system("make -C /opt/anyshell clean && make -C /opt/anyshell -j8");
            system("sudo systemctl restart anyshell-daemon.service");
            system("sudo systemctl restart anyshell-server.service");
        } else if (strcmp(argv[1], "change") == 0) {
            system("/opt/anyshell/change-database.sh");
        } else if (strcmp(argv[1], "reset") == 0) {
            MYSQL *conn;
            MYSQL_RES *res;
            MYSQL_ROW row;
            for (auto const &database : databases) {
                cout << "reseting database..." << endl;
                anyshell_server.database = database.c_str();
                conn = mysql_connection_setup(anyshell_server);
                sprintf(sql_query, "UPDATE hosts SET `requested`='0' WHERE 1;");
                res = mysql_run(conn, sql_query);
                mysql_close(conn);
            }
        } else {
            print_help();
        }
    }
    return 0;
}
