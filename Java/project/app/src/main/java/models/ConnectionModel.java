package models;

public class ConnectionModel extends Model{
    private final Integer interface_id1;
    private final Integer interface_id2;

    public ConnectionModel(String table_name, Integer id, Integer interface_id1, Integer interface_id2) {
        super(table_name, id);
        this.interface_id1 = interface_id1;
        this.interface_id2 = interface_id2;
    }

    public Integer getInterfaceId1() {
        return interface_id1;
    }


    public Integer getInterfaceId2(){
        return interface_id2;
    }
}