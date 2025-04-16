package models;
public class EquipmentModel extends Model{
    private String name;
    private final Integer max_interface_count;
    private final Integer company_id;

    public EquipmentModel(String table_name, Integer id, String name, Integer max_interface_count,  Integer company_id) {
        super(table_name, id);
        this.name = name;
        this.max_interface_count = max_interface_count;
        this.company_id = company_id;
    }

    
    public String getName() {
        return name;
    }

    public Integer getMaxInterfaceCount(){
        return max_interface_count;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Integer getCompany() {
        return company_id;
    }

}