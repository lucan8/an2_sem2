package models;
public class PacketModel extends Model{
    private final Integer interface_id;
    
    private String data;

    public PacketModel(String table_name, Integer id, Integer interface_id, String data){
        super(table_name, id);
        this.interface_id = interface_id;
        this.data = data;
    }

    public Integer getInterfaceId(){
        return interface_id;
    }
}