package models;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import db_conn.DBConn;
public abstract class Model{
    private final String table_name;
    private final Integer id;

    public Model(String table_name, Integer id){
        this.table_name = table_name;
        this.id = id;
    }

    public String getTableName(){
        return table_name;
    }

    public Integer getId() {
        return id;
    }

    public boolean insert(HashMap<String, Object> col_val_map) throws SQLException{
        StringBuilder columns = new StringBuilder(),
                      placeholders = new StringBuilder();
        String sep = ", ";
        ArrayList<Object> parameters = new ArrayList();

        // Set strings used in query
        for (Map.Entry<String, Object> entry : col_val_map.entrySet()){
            columns.append(entry.getKey()).append(sep);
            placeholders.append("?").append(sep);
            parameters.add(entry.getValue());
        }

        // Ignore the last ", "
        columns.setLength(columns.length() - sep.length());
        placeholders.setLength(placeholders.length() - sep.length());

        // Create and execute parameterized query
        String query = String.format("INSERT INTO %s (%s) VALUES(%s)", table_name, columns, placeholders);
        try(PreparedStatement stmt = DBConn.Instance().prepareStatement(query)){
            for (int i = 0; i < parameters.size(); ++i)
                stmt.setObject(i + 1, parameters.get(i));

            return stmt.executeUpdate(query) > 0;
        }
    }

    public boolean delete(HashMap<String, Object> where_cond) throws SQLException{
        StringBuilder where_str = new StringBuilder();
        String sep = " AND ";
        ArrayList<Object> parameters = new ArrayList();

        // Set strings used in query
        for (Map.Entry<String, Object> entry : where_cond.entrySet()){
            where_str.append(entry.getKey()).append(" = ?").append(sep);
            parameters.add(entry.getValue());
        }

        // Ignore the last sep
        where_str.setLength(where_str.length() - sep.length());

        // Create and execute parameterized query
        String query = String.format("DELETE FROM %s WHERE %s", table_name, where_str);
        try(PreparedStatement stmt = DBConn.Instance().prepareStatement(query)){
            for (int i = 0; i < parameters.size(); ++i)
                stmt.setObject(i + 1, parameters.get(i));
            
            return stmt.executeUpdate() > 0;
        }
    }

    public boolean update(HashMap<String, Object> col_val_map, HashMap<String, Object> where_cond) throws SQLException{
        StringBuilder update_str = new StringBuilder(),
                      where_str = new StringBuilder();
        String update_sep = ", ";
        String where_sep = " AND ";
        ArrayList<Object> parameters = new ArrayList<>();

        // Create update string and add part of params
        for (Map.Entry<String, Object> update_entry : col_val_map.entrySet()){
            update_str.append(update_entry.getKey()).append(" = ?").append(update_sep);
            parameters.add(update_entry.getValue());
        }

        // Create where string and add rest of params
        for (Map.Entry<String, Object> where_entry : where_cond.entrySet()){
            where_str.append(where_entry.getKey()).append(" = ?").append(where_sep);
            parameters.add(where_entry.getValue());
        }

        // Ignore last sep
        update_str.setLength(update_str.length() - update_sep.length());
        where_str.setLength(where_str.length() - where_sep.length());

        // Create and execute parameterized query
        String query = String.format("UPDATE %s SET %s WHERE %s", table_name, update_str, where_str);
        try(PreparedStatement stmt = DBConn.Instance().prepareStatement(query)){
            for (int i = 0; i < parameters.size(); ++i)
                stmt.setObject(i + 1, parameters.get(i));

            return stmt.executeUpdate() > 0;
        }
    }

    public ResultSet select(ArrayList<String> col_list, HashMap<String, Object> where_cond) throws SQLException{
        StringBuilder where_str = new StringBuilder();
        String col_str = String.join(", ", col_list);
        String sep = " AND ";
        ArrayList<Object> parameters = new ArrayList<>();

        for (Map.Entry<String, Object> where_entry : where_cond.entrySet()){
            where_str.append(where_entry.getKey()).append(" = ?").append(sep);
            parameters.add(where_entry.getValue());
        }

        where_str.setLength(where_str.length() - sep.length());
        String query = String.format("SELECT %s FROM %s WHERE %s", col_str, table_name, where_str);

        try(PreparedStatement stmt = DBConn.Instance().prepareStatement(query)){
            for (int i = 0; i < parameters.size(); ++i)
                stmt.setObject(i + 1, parameters.get(i));
            
            return stmt.executeQuery();
        }
    }   
}