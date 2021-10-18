#include <mysql/mysql.h>
#include <iostream>

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
    anyshell_server.database = "anyshell";

    conn = mysql_connection_setup(anyshell_server);
    res = mysql_run(conn, "select * from server;");

    while ((row = mysql_fetch_row(res)) != NULL){
        // the below row[] parametes may change depending on the size of the table and your objective
        std::cout << row[0] << " | " << row[1] << " | " << row[2] << " | " << row[3] << " | " << row[4] << std::endl;
    }

    mysql_free_result(res);
    mysql_close(conn);

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