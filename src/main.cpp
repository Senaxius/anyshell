#include "main.h"

int main(int argc, char **argv) {
    setting_ctrl_c();
    // get config from config.txt
    get_server_config("/opt/anyshell/etc/config.txt", &anyshell_server);
    // get config from user.txt
    get_user_config(&anyshell_user);
    // get databases from databases.txt
    get_databases(databases);

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
            // check for flags
            int ssh_connect = 1;
            int force_server = 0;
            int list_all = 0;
            for (int i = 2; i < argc; i++) {
                if (strcmp(argv[i], "-n") == 0) {
                    ssh_connect = 0;
                }
                if (strcmp(argv[i], "-s") == 0) {
                    force_server = 1;
                }
                if (strcmp(argv[i], "-v") == 0) {
                    list_all = 1;
                }
            }
            conn = mysql_connection_setup(anyshell_server);
            // list hosts
            print_hosts(conn, list_all);
            // get host ID to connect to
            cout << "\nWhich Host do you want to connect to? ";
            int input;
            cin >> input;
            // requesting host and getting host data
            request(conn, input, &anyshell_host);
            // keep request online
            thread updater(request_update, anyshell_server, anyshell_host);
            updater.detach();

            // waiting for host to accept request
            cout << "scanning if host is up: " << flush;
            time_t start = time(nullptr);
            time_t now;
            int i = 0;
            int b = 0;
            while (true) {
                i++;
                if (i == 50) {
                    i = 0;
                    cout << "." << flush;
                }
                now = time(nullptr);

                // check if host is up
                sprintf(sql_query, "SELECT * FROM connections WHERE ID='%s';", anyshell_host.ID);
                res = mysql_run(conn, sql_query);
                while ((row = mysql_fetch_row(res)) != NULL) {
                    // host is up
                    strcpy(anyshell_host.server_port, row[3]);
                    cout << "found!\n"
                         << endl;
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


            if (strcmp(anyshell_user.publicIP, anyshell_host.publicIP) == 0 && force_server == 0 && ssh_connect == 1) {
                // connect locally
                cout << "requested host is on same network, connecting localy..." << endl;
                connect(anyshell_host.user, anyshell_host.localIP, anyshell_host.host_port);
            } else {
                // connect remotely
                cout << "connecting to host via sever..." << endl;
                // kill old unused sessions
                sprintf(command, "if [ -S /opt/anyshell/etc/guest_socket_%s ]; then echo 1; else echo 0; fi", anyshell_user.port);
                if (exec(command) == "1") {
                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit %s", anyshell_user.port, anyshell_server.domain);
                    system(command);
                    sprintf(command, "rm -f /opt/anyshell/etc/guest_socket_%s", anyshell_user.port);
                    system(command);
                }
                // get user port 
                sprintf(anyshell_user.port, "%i", (atoi(anyshell_host.server_port) - 1000));
                // tunnel to server
                sprintf(command, "ssh -f -N -T -M -S /opt/anyshell/etc/guest_socket_%s %s@%s -p %s -i ~/.ssh/anyshell-key -L %s:localhost:%s", anyshell_user.port, anyshell_server.user, anyshell_server.domain, anyshell_server.SSH_port, anyshell_user.port, anyshell_host.server_port);
                system(command);
                if (ssh_connect == 1) {
                    // connect to tunnel
                    strcpy(ssh_hostname, "localhost");
                    connect(anyshell_host.user, ssh_hostname, anyshell_user.port);

                    // kill tunnel after connection
                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost || rm -f /opt/anyshell/etc/guest_socket_%s", anyshell_user.port, anyshell_user.port);
                    system(command);
                } else {
                    sprintf(command, "ssh %s@localhost -p %s", anyshell_host.user, anyshell_user.port);
                    cout << "\nTo connect, run: \n"
                         << "----------------------------------------------------\n"
                         << command << "\n----------------------------------------------------" << endl;
                    cout << "\nPress any key to stop..." << endl;
                    cin.ignore();
                    cin.ignore();

                    sprintf(command, "ssh -S /opt/anyshell/etc/guest_socket_%s -O exit localhost || rm -f /opt/anyshell/etc/guest_socket_%s", anyshell_user.port, anyshell_user.port);
                    system(command);
                }
            }
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
                vector<thread> hosting_threads;
                int sshd = 0;
                while (1) {
                    if (sshd == 0 && strcmp(exec("sudo systemctl is-active sshd.service").c_str(), "active") == 0 && anyshell_user.ssh_enabled == 0) {
                        cout << "deactivating sshd.service" << endl;
                        system("sudo systemctl stop sshd.service");
                    }
                    for (auto const &database : databases) {
                        anyshell_server.database = database.c_str();
                        conn = mysql_connection_setup(anyshell_server);
                        // update host entry
                        get_user_config(&anyshell_user);
                        sql_update(conn, &anyshell_user);

                        sprintf(sql_query, "SELECT * FROM requests WHERE Name='%s';", anyshell_user.hostname);
                        res = mysql_run(conn, sql_query);
                        while ((row = mysql_fetch_row(res)) != NULL) {
                            if (check_connection(&connections, atoi(row[0])) == 0) {
                                // found new request
                                connections.push_back(atoi(row[0]));
                                // start host thread
                                thread th(host, atoi(row[0]), atoi(row[3]), &anyshell_user, anyshell_server, &connections, &sshd);
                                // thread th(host, atoi(row[0]), atoi(row[3]), &anyshell_user, anyshell_server);
                                th.detach();
                                hosting_threads.push_back(move(th));
                            }
                        }
                        mysql_free_result(res);
                        mysql_close(conn);
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
                    cout << "updating list on database: " << database << endl;
                    anyshell_server.database = database.c_str();
                    conn = mysql_connection_setup(anyshell_server);
                    // update host online status
                    sprintf(sql_query, "UPDATE hosts SET `online`='0' WHERE `last-online` <  (NOW() - INTERVAL 10 SECOND);");
                    res = mysql_run(conn, sql_query);

                    // delete old requests
                    sprintf(sql_query, "SELECT * FROM requests WHERE `last-used` <  (NOW() - INTERVAL 10 SECOND);");
                    res = mysql_run(conn, sql_query);
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        cout << "Found unused connection with ID: " << row[0] << ", killing it..." << endl;
                        MYSQL *conn2;
                        MYSQL_RES *res2;
                        MYSQL_ROW row2;
                        conn2 = mysql_connection_setup(anyshell_server);
                        sprintf(sql_query, "DELETE FROM requests WHERE `ID`=%s;", row[0]);
                        res2 = mysql_run(conn2, sql_query);
                        sprintf(sql_query, "DELETE FROM connections WHERE `ID`=%s;", row[0]);
                        res2 = mysql_run(conn2, sql_query);
                        mysql_close(conn2);
                    }
                    mysql_close(conn);
                }
                sleep(1);
            }
/*****************************reload****************************/
        } else if (strcmp(argv[1], "upgrade") == 0) {
            string temp;
            int server = 0;
            int daemon = 0;
            cout << "Checking which services are active..." << endl;
            temp = exec("sudo systemctl is-active anyshell-server.service");
            if (temp == "active") {
                server = 1;
            }
            temp = exec("sudo systemctl is-active anyshell-daemon.service");
            if (temp == "active") {
                daemon = 1;
            }
            cout << "----------------------done----------------------" << endl;

            if (server == 1) {
                cout << "deactivate anyshell-server" << endl;
                system("sudo systemctl stop anyshell-server.service");
                cout << "----------------------done----------------------" << endl;
            }
            if (daemon == 1) {
                cout << "deactivate anyshell-daemon" << endl;
                system("sudo systemctl stop anyshell-daemon.service");
                cout << "----------------------done----------------------" << endl;
            }
            cout << "updating repository..." << endl;
            system("cd /opt/anyshell && git pull");
            cout << "----------------------done----------------------" << endl;
            cout << "compiling raw anyshell code..." << endl;
            system("make -C /opt/anyshell clean && make -C /opt/anyshell -j8");
            cout << "----------------------done----------------------" << endl;
            if (server == 1) {
                cout << "starting anyshell-server" << endl;
                system("sudo systemctl start anyshell-server.service");
                cout << "----------------------done----------------------" << endl;
            }
            if (daemon == 1) {
                cout << "starting anyshell-daemon" << endl;
                system("sudo systemctl start anyshell-daemon.service");
                cout << "----------------------done----------------------" << endl;
            }
/*****************************change****************************/
        } else if (strcmp(argv[1], "change") == 0) {
            system("/opt/anyshell/etc/change-database.sh");
/*************************nothing found*************************/
        } else {
            cout << "Speek German to me, I can't understand shit :(";
            print_help();
        }
    }
    return 0;
}
