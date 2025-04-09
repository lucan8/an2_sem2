import java.io.*;
import java.util.HashMap;

public class Request{
    private HashMap<String, String> header;
    private HashMap<String, String> query_params;
    private HashMap<String, String> form_params;

    // Reads request using BufferedReader and construct the object
    public Request(BufferedReader in) throws IOException{
        // Parse header
        String curr_line = in.readLine();
        if (curr_line.isEmpty())
            return;
        
        // First line is method and route
        String[] split_line = curr_line.split(" ");

        header.put("method", split_line[0]);
        ParseRoute(split_line[1]);
        
        // Parse Header
        while (!curr_line.isEmpty()){
            String[] split_data = curr_line.split(":", 2);

            header.put(split_data[0], split_data[1]);
            curr_line = in.readLine();
        }

        // Parse Form parameters
        curr_line = in.readLine();
        ParseParams(curr_line);
    }

    // Splits the route and query parameters and sets them
    private void ParseRoute(String route){
        String[] split_route = route.split("\\?", 2);
        header.put("route", split_route[0]);

        query_params = ParseParams(split_route[1]);
    }

    // Parses string containing query/form parameters
    private HashMap<String, String> ParseParams(String param_string){
        String[] param_pairs = param_string.split("&");
        HashMap<String, String> params = new HashMap<>();

        for (String p_pair : param_pairs){
            String[] split_pair = p_pair.split("=", 2);
            params.put(split_pair[0], split_pair[1]);
        }

        return params;
    }

    public String GetHeader(String h_key) {return header.get(h_key);}
    public String GetQueryParam(String p_key) {return query_params.get(p_key);} 
    public String GetFormParam(String p_key) {return form_params.get(p_key);} 
}