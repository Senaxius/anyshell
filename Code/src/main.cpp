#include <mysql/mysql.h>
#include <iostream>
#include <string.h>

using namespace std;

struct connection_details
{
    const char *server, *user, *password, *database;
};

MYSQL* mysql_connection_setup(struct connection_details mysql_details);
MYSQL_RES* mysql_run(MYSQL *connection, const char *sql_query);

int main(int argc, char **argv){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    struct connection_details anyshell_server;
    anyshell_server.server = "192.168.178.21";
    anyshell_server.user = "senaex";
    anyshell_server.password = "Quande-0918";
    anyshell_server.database = "senaex";

    // conn = mysql_connection_setup(anyshell_server);
    // res = mysql_run(conn,
    //     "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, `publicIP`, `localIP`, `online`, `last-online`, `requested`) "
    //     "VALUES ('0', 'Arch-PC', 'senaex', '22', '87.98.45.78', '192.168.178.21', '1', '2038-01-19 03:14:07', '0');");
    // res = mysql_run(conn, "DELETE FROM server WHERE ID='12'");
    // res = mysql_run(conn, "SELECT * FROM hosts;");
    // res = mysql_run(conn, "SHOW TABLES");
    // res = mysql_run(conn, "SELECT * FROM server WHERE User = 'lennart';");
    // res = mysql_run(conn, "SELECT * FROM server WHERE User LIKE 'len%' AND Domain LIKE 'no%';");
    // res = mysql_run(conn, "SELECT * FROM server WHERE (User LIKE 'sen%') OR (User LIKE 'len%');");
    // res = mysql_run(conn, "SELECT * FROM server WHERE User = 'senaex';");
    // res = mysql_run(conn, 
    //     "UPDATE hosts "
    //     "SET localIP='192.197.178.156'"
    //     "WHERE ID=1"
    // );
    // ID, Name, User, Port, publicIP, localIP, requested, last-online, online 
    // while ((row = mysql_fetch_row(res)) != NULL){
    //     // the below row[] parametes may change depending on the size of the table and your objective
    //     std::cout << row[0] << " | " << row[1] << " | " << row[2] << " | " << row[3] << " | " << row[4] << std::endl;
    // }


    // mysql_free_result(res);
    // mysql_close(conn);

    if(argc > 1){
        if (strcmp(argv[1], "list") == 0) {
            // cout << "Listing all hosts...\n" << endl;
            conn = mysql_connection_setup(anyshell_server);
            res = mysql_run(conn, "SELECT * FROM hosts;");
            printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n", "ID", "Hostname", "User", "Port", "public-IP", "local-IP", "online");
            while ((row = mysql_fetch_row(res)) != NULL){
                printf("%-3s | %-10s | %-8s | %-4s | %-15s | %-15s | %s \n", row[0], row[1], row[2], row[3], row[4], row[5] ,row[6]);
            }
        }
    }

    // res = mysql_run(conn, "INSERT INTO `server` (`ID`, `User`, `Domain`, `local-IP`, `Port`) VALUES ('3', 'lennart', 'noftp.ddns.net', '54', '41999');");
    return 0;
}

// system("ssh senaex@192.168.178.21 -p 41999");

MYSQL* mysql_connection_setup(struct connection_details mysql_details){
    MYSQL *connection = mysql_init(NULL);

    //connect database
    if(!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)){
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1); 
    }
    return connection;
}

MYSQL_RES* mysql_run(MYSQL *connection, const char *sql_query){
    //send query to db
    if(mysql_query(connection, sql_query)){
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return mysql_use_result(connection);
}