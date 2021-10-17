#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include "/usr/include/mysql/mysql.h"
using namespace std;

struct connection_detail;
MYSQL* mysql_connection_setup(struct connection_details mysql_details);
MYSQL_RES* mysql_perform_query(MYSQL *connection, const char *sql_query);


int main(int argc, char **argv){
    MYSQL *con;	// the connection
    MYSQL_RES *res;	// the results
    MYSQL_ROW row;	// the results rows (array)

    struct connection_details mysqlD;
    mysqlD.server = "192.168.178.21";  // where the mysql database is
    mysqlD.user = "senaex"; // user
    mysqlD.password = "Quande-0918"; // the password for the database
    mysqlD.database = "anyshell";	// the databse

    // connect to the mysql database
    con = mysql_connection_setup(mysqlD);

    // get the results from executing commands
    res = mysql_perform_query(con, "select * from server;");

    std::cout << ("Database Output:\n") << std::endl;

    while ((row = mysql_fetch_row(res)) != NULL){
        // the below row[] parametes may change depending on the size of the table and your objective
        std::cout << row[0] << " | " << row[1] << " | " << row[2] << " | " << row[3] << " | " << row[4] << std::endl << std::endl;
    }

    // clean up the database result
    mysql_free_result(res);
    
    // close database connection
    mysql_close(con);

    return 0;
}

// system("ssh senaex@192.168.178.21 -p 41999");

struct connection_details
{
    const char *server, *user, *password, *database;
}

MYSQL* mysql_connection_setup(struct connection_details mysql_details){
    MYSQL *connection = mysql_init(NULL); // mysql instance
    
    //connect database
    if(!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)){
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1); 
    }

    return connection;
}

// mysql_res = mysql result
MYSQL_RES* mysql_perform_query(MYSQL *connection, const char *sql_query){
    //send query to db
    if(mysql_query(connection, sql_query)){
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }

    return mysql_use_result(connection);
}
