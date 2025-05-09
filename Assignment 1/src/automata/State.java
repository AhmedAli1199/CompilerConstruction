package automata;

public class State {
    private int id;
    private boolean isAccepting;

    public State(int id, boolean isAccepting) {
        this.id = id;
        this.isAccepting = isAccepting;
    }

    public int getId() {
        return id;
    }

    public boolean isAccepting() {
        return isAccepting;
    }
    
	public void setAccepting(boolean isAccepting) {
		this.isAccepting = isAccepting;
	}
	
	public void setId(int id) {
		this.id = id;
	}
	
	
}