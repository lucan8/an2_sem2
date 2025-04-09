
import java.io.IOException;
import java.net.ServerSocket;

public class Server{
    public static void main(String[] args){
        ServerSocket server;
        try {
            server = new ServerSocket(8080);
            System.out.println("Listening on port 8080");   
        } catch (IOException e) {
            System.err.println("[ERROR]: Server creation: " + e.getMessage());
            return;
        }

        while (true) {
            new Thread(() -> {
                try{
                    ClientHandler.handle(server.accept());
                }catch (Exception e){
                    System.err.println("[ERROR]: Handling request for client socket failed: " + e.getMessage());
                }
            }).start();   
        }
    }
}