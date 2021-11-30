#include "main.h"

MYSQL *mysql_connection_setup(struct server_details mysql_details) {
    MYSQL *connection = mysql_init(NULL);

    // connect database
    if (!mysql_real_connect(connection, mysql_details.domain,
                            mysql_details.user, mysql_details.password,
                            mysql_details.database, atoi(mysql_details.SQL_port), NULL, 0)) {
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

void sql_update(MYSQL *conn, user_details *user_details) {
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "`last-online`=CURRENT_TIMESTAMP, "
            "`online`='1', "
            "`localIP`='%s', "
            "`publicIP`='%s' "
            "WHERE Name='%s';",
            user_details->localIP, user_details->publicIP, user_details->hostname);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);
}

void request(MYSQL *conn, int host_ID, host_details *host_details){
    // get host details
    sprintf(sql_query, "SELECT * FROM hosts WHERE ID='%i';", host_ID);
    res = mysql_run(conn, sql_query);
    while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(host_details->hostname, row[1]);
        strcpy(host_details->user, row[2]);
        strcpy(host_details->host_port, row[3]);
        strcpy(host_details->publicIP, row[4]);
        strcpy(host_details->localIP, row[5]);
    }
    mysql_free_result(res);
    // get unique request ID
    get_ID(conn, "requests", host_details->ID);
    // request host in requests table
    sprintf(sql_query,
            "INSERT INTO requests (`ID`, `Name`, `User`, `Port`, `last-used`) " 
            "VALUES ('%s', '%s', '%s', '%s', CURRENT_TIMESTAMP);", 
            host_details->ID, host_details->hostname, host_details->user, host_details->host_port);
    res = mysql_run(conn, sql_query);
}
void request_update(server_details server_details, host_details host_details){
    conn = mysql_connection_setup(server_details);
    while (1){
        sprintf(sql_query,
                "UPDATE requests "
                "SET "
                "`last-used`=CURRENT_TIMESTAMP "
                "WHERE ID=%s;",
                host_details.ID);
        res = mysql_run(conn, sql_query);
        mysql_free_result(res);
        sleep(2);
    }
}

void unrequest(MYSQL *conn, host_details *host_details){
    sprintf(sql_query,
            "DELETE FROM requests "
            "WHERE ID='%s';",
            host_details->ID);
    res = mysql_run(conn, sql_query);
}

void host(int ID, int port, user_details *user_details, server_details server_details, list<int> *connections, int *sshd){
// void host(int ID, int port, user_details *user_details, server_details server_details){
    conn = mysql_connection_setup(server_details);
    cout << "Hosting Port: " << port << " on ID: " << ID << endl;
    // stoping main loop from stoping sshd service
    *sshd += 1;
    // start sshd.service if not running already
    if (strcmp(exec("sudo systemctl is-active sshd.service").c_str(), "inactive") == 0) {
        cout << "start sshd.service" << endl;
        system("sudo systemctl start sshd.service");
    }

    int server_port;
    char temp[6];
    char socket[100];
    get_ID(conn, "connections", temp);
    server_port = (41999 + atoi(temp));

    sprintf(command, "pkill -f %i", server_port);

    sprintf(socket, "/opt/anyshell/etc/host_socket_%i", ID);
    cout << 
    sprintf(command, "ssh -f -N -T -M -S %s -R %i:localhost:%i %s@%s -p %s -i ~/.ssh/anyshell-key ", socket, server_port, port, server_details.user, server_details.domain, server_details.SSH_port);
    system(command);

    sprintf(sql_query,
            "INSERT INTO connections (`ID`, `Name`, `Host-Port`, `Server-Port`) "
            "VALUES ('%i', '%s', '%i', '%i');",
            ID, user_details->hostname, port, server_port);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);
    while (1){
        // check if host is still in requests table
        sprintf(sql_query,
                "SELECT * FROM requests WHERE ID='%i';",
                ID);
        res = mysql_run(conn, sql_query);
        if ((row = mysql_fetch_row(res)) == NULL) {
            // host is no longer in requests table
            cout << "Stop hosting Port: " << port << " on ID: " << ID << endl;
            // deleting host from connections table
            sprintf(sql_query,
            "DELETE FROM connections "
            "WHERE `ID`='%i';"
            , ID);
            res = mysql_run(conn, sql_query);

            // closeing ssh connection
            sprintf(command, "ssh -S %s -O exit %s &>/dev/null", socket, server_details.domain);
            system(command);
            sprintf(command, "pkill -f %i", server_port);
            system(command);

            // remove the ID from connections list
            remove_from_list(connections, ID);
            *sshd -= 1;
            break;
        }
        mysql_free_result(res);
        sleep(1);
    }
}

void connect(char *user, char *host, char *port){
    sprintf(command, "/opt/anyshell/lib/connect.sh %s %s %s", user, host, port);
    system(command);
}