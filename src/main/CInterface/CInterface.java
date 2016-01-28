/**
 * Created by jackfarrelly on 27/01/2016.
 */
public class CInterface {
    private native String getVersion();
    private native String getBpm(String filename);

    static {
        System.loadLibrary("kitchensync");
    }

    public static void main(String[] args) {
        CInterface cInterface = new CInterface();

        String version = cInterface.getVersion();
        System.out.println("Soundtouch lib version: " + version);
        System.out.println("BPM of /tmp/tmp.wav: " + cInterface.getBpm("/tmp/tmp.wav"));
    }
}
