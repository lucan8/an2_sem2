package db_conn;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
public class DBConn{
    private static Connection instance = null;
    private DBConn(){}

    public static void main(String[] args) {
        try{
            if (TestConn())
                System.out.println("[INFO] Connected to database succesfully");
            else
                System.out.println("[ERROR] Connected to database unsuccesfully");
        } catch(SQLException e){
            System.err.println("[ERROR] Testing the database connection: " + e.getMessage());
        }
    }

    // Returns the database connection object(and sets it if needed)
    public static Connection Instance() throws SQLException{
        if (instance == null){
            String url = "jdbc:mysql://localhost:3306/packet_tracer_2";
            String user = "root";
            String password = "lucstef8";

            instance = DriverManager.getConnection(url, user, password);
        }
        return instance;
    }

    public static boolean TestConn() throws SQLException{
        Connection inst = Instance();

        return inst.isValid(2);
    }
}