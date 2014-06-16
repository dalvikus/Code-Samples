#!/usr/bin/python

import MySQLdb

# Assure only one connection is established using the following variable
MySQLConn = None

class MySQLWrapper(object):
    def __init__(self, Host = "####", User = "####", Password = "####"):
        global MySQLConn
        if MySQLConn:
#           print "TRACE: __init__: Using..."
            pass
        else:
#           print "TRACE: __init__: Connecting..."
            conn = None
            try:
                conn = MySQLdb.connect(
                    host = Host,
                    user = User,
                    passwd = Password,
                    db = "dbMolecule"
                )
            except MySQLdb.Error, e:
                print "Error: MySQL: Error Code = %d(|%s|)" % (e.args[0], e.args[1])
            if conn:
                MySQLConn = conn

    def getMySQLConn(self):
        return MySQLConn

    def __del__(self):
        if MySQLConn:
#           print "TRACE: __del__: Closing..."
            return
            try:
                MySQLConn.close()
            except MySQLdb.Error, e:
                print "Error: MySQL: Error Code = %d(|%s|)" % (e.args[0], e.args[1])
        else:
#           print "TRACE: __del__: None"
            pass

if __name__ == "__main__":
    import sys

    MySQLInstance = MySQLWrapper()
    conn = MySQLInstance.getMySQLConn()
    if not conn:
        print "Error: There is no connection to MySQL Server"
        sys.exit(1)     # MySQL Connection Error

########################################################################
    try:
#       cursor = conn.cursor()
        cursor = conn.cursor(MySQLdb.cursors.DictCursor)
        cursor.execute("SELECT Host, User, Password FROM user")
        print "# of rows: %d" % cursor.rowcount
        while (1):
            row = cursor.fetchone()
            if row == None:
                break;
#           print "%s %s %s" % (row[0], row[1], row[2])
            print "Host:= |%s|, User:= |%s|, Password:= |%s|" % (row["Host"], row["User"], row["Password"])

        conn.commit()

    except MySQLdb.Error, e:
        print "Error %d: %s" % (e.args[0], e.args[1])
        sys.exit(2)     # MySQL Example Error
########################################################################
    try:
        print MySQLConn         # MySQLConn is hidden, so cannot be accessed directly!
    except NameError:
        print "Error: Cannot access the variable 'MySQLConn' directly!"

    sys.exit(0)         # Everything is ok!
