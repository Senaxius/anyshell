#include "main.h"

void connect(char *user, char *host, char *port){
    sprintf(command, "/opt/anyshell/lib/connect.sh %s %s %s", user, host, port);
    system(command);
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

    sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database, port);
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

    sprintf(socket, "/opt/anyshell/etc/host_socket_%s_%i", database, port);
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