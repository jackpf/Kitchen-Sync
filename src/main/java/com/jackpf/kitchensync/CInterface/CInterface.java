package com.jackpf.kitchensync.CInterface;

/**
 * Created by jackfarrelly on 27/01/2016.
 */
public class CInterface {
    public native String getVersion();
    public native float getBpm(String filename);
    public native void setBpm(String filename, float fromBpm, float toBpm);

    static {
        System.loadLibrary("kitchensync");
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            throw new RuntimeException("Not enough args");
        }

        CInterface cInterface = new CInterface();

        String version = cInterface.getVersion();
        System.out.println("Soundtouch lib version: " + version);
        System.out.println("BPM of " + args[0] + ": "  + cInterface.getBpm(args[0]));
    }
}
