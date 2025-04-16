package models;

public class EquipmentInterfaceModel extends Model{
    private String ip;
    private final String mac_addr;
    private String mask;
    private final Integer equipment_id;

    //Generate constructor with all parameters

    public EquipmentInterfaceModel(String table_name, Integer id, String ip, String mac_addr, String mask, Integer equipment_id) {
        super(table_name, id);
        this.ip = ip;
        this.mac_addr = mac_addr;
        this.mask = mask;
        this.equipment_id = equipment_id;
    }

    public String getIp() {
        return ip;
    }

    public String getMac_addr() {
        return mac_addr;
    }

    public String getMask() {
        return mask;
    }

    public Integer getEquipmentId() {
        return equipment_id;
    }


    public void setIp(String ip) {
        this.ip = ip;
    }

    public void setMask(String mask) {
        this.mask = mask;
    }
}