import java.io.*;
import java.lang.reflect.*;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;

public class ClientHandler{
    // route: controller_method
    private static final HashMap<String, Method> routing_table = new HashMap<>();

    static{
        try{
            routing_table.putAll(Map.ofEntries(
                Map.entry("/index", DefaultController.class.getMethod("index"))
            ));
        } catch (NoSuchMethodException e){
            System.err.println("[ERORR]: Creating the routing table: " + e.getMessage());
        }
    }

    public static void handle(Socket socket) throws IOException, IllegalAccessException, InvocationTargetException{
        // Open input output streams
        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        OutputStream out = socket.getOutputStream();

        // Create request object using InputStream
        Request req = new Request(in);

        // Route the request to the correct controller
        Method route_handler = routing_table.get(req.GetHeader("route"));
        if (route_handler != null)
            route_handler.invoke(null, req, out);
        else
            handle404();

        // Close socket
        socket.close();
    }

    private static void handle404(){}

}