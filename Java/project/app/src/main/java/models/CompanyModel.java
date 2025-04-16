package models;
public class CompanyModel extends Model{
    private String name;

    // TODO: add creation date
    public CompanyModel(String table_name, Integer id, String name){
        super(table_name, id);
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}