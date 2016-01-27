/**
 * Created by jackfarrelly on 27/01/2016.
 */
public class CInterface {
    private native String getBpm();

    static {
        System.loadLibrary("soundtouch");
    }

    public static void main(String[] args) {
        CInterface cInterface = new CInterface();
        String bpm = cInterface.getBpm();
        System.out.println("bpm = " + bpm);
    }
}
