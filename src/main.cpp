#include "main.h"

int main(int argc, char **argv) {
    get_server_config("/opt/anyshell/etc/config.txt", &anyshell_server);  //get config from config.txt
    get_user_config(&anyshell_user);                                      //get config from user.txt
    get_databases(databases);                                             //get databases from databases.txt

    if (argc == 1) {
        print_help();
    } else {
/*****************************list******************************/
        if (strcmp(argv[1], "list") == 0) {
            conn = mysql_connection_setup(anyshell_server);
            if (argc > 2) {
                if (strcmp(argv[2], "-v") == 0) {
                    print_hosts(conn, 1);
                }
            } else {
                print_hosts(conn, 0);
            }
            mysql_close(conn);
/****************************connect****************************/
        } else if (strcmp(argv[1], "connect") == 0) {
            conn = mysql_connection_setup(anyshell_server);
            if (argc > 2) {
                if (strcmp(argv[2], "-v") == 0) {
                    print_hosts(conn, 1);
                }
            } else {
                print_hosts(conn, 0);
            }
            // get host ID to connect to
            cout << "\nWhich Host do you want to connect to? ";
            int input;
            cin >> input;
            // requesting host and getting host data
            request(conn, input, &anyshell_host);
            // waiting for host to accept request

            cout << "scanning if host is up: " << flush;
            time_t start = time(nullptr);
            time_t now;
            int i = 0;
            int b = 0;
            while (true) {
                i++;
                if (i == 60){
                    i = 0;
                    cout << "." << flush;
                }
                now = time(nullptr);

                //check if host is up
                sprintf(sql_query, "SELECT * FROM connections WHERE ID='%s';", anyshell_host.ID);
                res = mysql_run(conn, sql_query);
                while ((row = mysql_fetch_row(res)) != NULL) {
                    //host is up
                    cout << "found!\n" << endl;
                    b = 1;
                    break;
                }
                mysql_free_result(res);

                // if host is found, break
                if (b == 1) {
                    break;
                }
                // if host is not found, break after 5 seconds
                if (now - start > 4) {
                    cout << "\nCould not connect to Host!" << endl;
                    unrequest(conn, &anyshell_host);
                    exit(0);
                }
            }

            // char own_IP[20];
            // int ssh_connect = 1;
            // if (argc > 2) {
            //     if (strcmp(argv[2], "-n") == 0) {
            //         ssh_connect = 0;
            //     }
            //     if (strcmp(argv[2], "-s") == 1) {
            //         get_publicIP(own_IP);
            //     }
            // } else {
            //     get_publicIP(own_IP);
            // }

            // if (ssh_connect == 1) {
            //     if (strcmp(own_IP, publicIP) == 0) {
            //         cout << "requested host is on same network, connecting localy..." << endl;
            //         connect(user, localIP, port);
            //     } else {
            //         cout << "connecting to host via sever..." << endl;
            //         sprintf(command, "lsof -t -i:%s >/dev/null && lsof -ti:%s | xargs kill -9", server_port, server_port);
            //         system(command);
            //         sprintf(command, "rm -f /opt/anyshell/etc/guest_socket_%s", server_port);
            //         system(command);
            //         sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket_%s %s@%s -p %s -i ~/.ssh/anyshell-key -L %s:localhost:%s", socket_number, server_user, server_domain, server_ssh_port, server_port, server_port);
            //         system(command);

            //         strcpy(hostname, "localhost");
            //         connect(user, hostname, server_port);

            //         sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket_%s", server_port, server_port);
            //         system(command);
            //     }
            // } else {
            //     if (strcmp(own_IP, publicIP) == 0) {
            //         cout << "requested host is on same network, connecting localy..." << endl;
            //         sprintf(command, "ssh %s@%s -p %s", user, localIP, port);
            //         cout << "\nTo connect, run: \n"
            //              << "----------------------------------------------------\n"
            //              << command << "\n----------------------------------------------------" << endl;
            //         cout << "\nPress any key to stop..." << endl;
            //         cin.ignore();
            //         cin.ignore();
            //     } else {
            //         cout << "connecting to host via sever..." << endl;
            //         sprintf(command, "lsof -t -i:%s >/dev/null && lsof -ti:%s | xargs kill -9", server_port, server_port);
            //         system(command);
            //         sprintf(command, "rm -f /opt/anyshell/etc/guest_socket_%s", server_port);
            //         system(command);
            //         sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket_%s %s@%s -p %s -i ~/.ssh/anyshell-key -L %s:localhost:%s", server_port, server_user, server_domain, server_ssh_port, server_port, server_port);
            //         system(command);

            //         sprintf(command, "ssh %s@localhost -p %s", user, server_port);
            //         cout << "\nTo connect, run: \n"
            //              << "----------------------------------------------------\n"
            //              << command << "\n----------------------------------------------------" << endl;
            //         cout << "\nPress any key to stop..." << endl;
            //         cin.ignore();
            //         cin.ignore();

            //         sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost >/dev/null || rm -f /opt/anyshell/etc/guest_socket_%s", server_port, server_port);
            //         system(command);
            //     }
            // }

            // // unrequest(conn, input);
            unrequest(conn, &anyshell_host);
            mysql_close(conn);

        } else if (strcmp(argv[1], "host") == 0) {
            /***************************host-setup**************************/
            if (strcmp(argv[2], "setup") == 0) {
                char port[5];
                cout << "Which Port do you want to share? ";
                cin >> port;
                get_user_config(&anyshell_user);

                conn = mysql_connection_setup(anyshell_server);
                res = mysql_run(conn, "SELECT * FROM hosts;");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    if (strcmp(row[1], anyshell_user.hostname) == 0) {
                        if (strcmp(row[2], anyshell_user.user) == 0) {
                            if (strcmp(row[3], port) == 0) {
                                cout << "\nError: host already exists" << endl;
                                mysql_free_result(res);
                                exit(1);
                            }
                        }
                    }
                }
                char ID[3];
                get_ID(conn, "hosts", ID);
                sprintf(sql_query,
                        "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, "
                        "`publicIP`, `localIP`, `online`, `last-online`, "
                        "`requested`) "
                        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '1', "
                        "CURRENT_TIMESTAMP, '0');",
                        ID, anyshell_user.hostname, anyshell_user.user, port, anyshell_user.publicIP, anyshell_user.localIP);

                mysql_free_result(res);

                res = mysql_run(conn, sql_query);

                sprintf(sql_query,
                        "SELECT * FROM hosts WHERE User='%s' AND Name='%s' AND "
                        "Port='%s';",
                        anyshell_user.user, anyshell_user.hostname, port);
                res = mysql_run(conn, sql_query);
                printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %s \n",
                       "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "online");
                while ((row = mysql_fetch_row(res)) != NULL) {
                    printf("%-3s | %-14s | %-8s | %-5s | %-15s | %-15s | %s \n",
                           row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
                }
                mysql_free_result(res);
                mysql_close(conn);
                /**************************host-remove**************************/
            } else if (strcmp(argv[2], "remove") == 0) {
                conn = mysql_connection_setup(anyshell_server);
                cout << "Which Port do you want to remove? ";
                char port[5];
                cin >> port;
                cout << "Removing this device from hosts, port: " << port << endl;

                get_user(anyshell_user.user);
                get_hostname(anyshell_user.hostname);

                sprintf(sql_query,
                        "DELETE FROM hosts "
                        "WHERE `Name`='%s' "
                        "AND `Port`='%s' "
                        "AND `User`='%s';",
                        anyshell_user.hostname, port, anyshell_user.user);
                res = mysql_run(conn, sql_query);
                mysql_free_result(res);
                mysql_close(conn);
                cout << "done!" << endl;
                /**************************host-daemon**************************/
            } else if (strcmp(argv[2], "daemon") == 0) {
                while (1) {
                    for (auto const &database : databases) {
                        anyshell_server.database = database.c_str();
                        conn = mysql_connection_setup(anyshell_server);
                        get_user_config(&anyshell_user);

                        sql_update(conn, &anyshell_user);
                        // sprintf(sql_query, "SELECT * FROM hosts WHERE Name='%s';", hostname);
                        // res = mysql_run(conn, sql_query);
                        // while ((row = mysql_fetch_row(res)) != NULL) {
                        //     if (strcmp(row[8], "0") == 0) {
                        //         // Host down
                        //         int port = atoi(row[3]);
                        //         cout << database.c_str() << ": \t"
                        //              << "Host down, port: " << port << "   " << ssh_enabled << endl;
                        //         sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database.c_str(), port);
                        //         int check = socket_check(socket);
                        //         if (check == 1) {
                        //             host_down(anyshell_server.database, port, server_domain);
                        //             if (ssh_enabled == 0) {
                        //                 system("systemctl stop sshd.service");
                        //             }
                        //         }
                        //     } else {
                        //         // Host up
                        //         int port = atoi(row[3]);
                        //         cout << database.c_str() << ": \t"
                        //              << "Host up, port: " << port << "   " << ssh_enabled << endl;
                        //         sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database.c_str(), port);
                        //         int check = socket_check(socket);
                        //         if (check == 0) {
                        //             if (ssh_enabled == 0) {
                        //                 system("systemctl start sshd.service");
                        //             }
                        //             host_up(anyshell_server.database, port, server_user, server_domain, server_ssh_port);
                        //         }
                        //     }
                        //     mysql_close(conn);
                    }
                    sleep(1);
                }
            }
/*****************************server****************************/
        } else if (strcmp(argv[1], "server") == 0) {
            cout << "starting anyshell server..." << endl;
            while (1) {
                // for each database the server is assigned
                for (auto const &database : databases) {
                    cout << "updating list..." << endl;
                    anyshell_server.database = database.c_str();
                    conn = mysql_connection_setup(anyshell_server);
                    // update host online status
                    sprintf(sql_query, "UPDATE hosts SET `online`='0' WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                    res = mysql_run(conn, sql_query);
                    // delete old requests
                    sprintf(sql_query, "DELETE FROM requests WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                    res = mysql_run(conn, sql_query);
                    mysql_close(conn);
                }
                sleep(1);
            }
/*****************************reload****************************/
        } else if (strcmp(argv[1], "upgrade") == 0) {
            string temp;
            int server = 0;
            int daemon = 0;
            temp = exec("sudo systemctl is-active anyshell-server.service");
            if (temp == "active"){
                server = 1;
            }
            temp = exec("sudo systemctl is-active anyshell-daemon.service");
            if (temp == "active"){
                daemon = 1;
            }

            if (server == 1){ system("sudo systemctl stop anyshell-server.service");}
            if (daemon == 1){ system("sudo systemctl stop anyshell-daemon.service");}
            system("cd /opt/anyshell && git pull");
            system("make -C /opt/anyshell clean && make -C /opt/anyshell -j8");
            if (server == 1){ system("sudo systemctl start anyshell-server.service");}
            if (daemon == 1){ system("sudo systemctl start anyshell-daemon.service");}
/*****************************change****************************/
        } else if (strcmp(argv[1], "change") == 0) {
            system("/opt/anyshell/change-database.sh");
/*****************************reset*****************************/
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
/*************************nothing found*************************/
        } else {
            cout << "Speek German to me, I can't understand shit :(";
            print_help();
        }
    }
    return 0;
}
